#pragma once
#include "../World/WorldDataAccess.h"
#include "../../Utils/Clock.h"
#include <unordered_set>
#include <thread>
#include <mutex>

class LightingEngine {
private:
	std::deque<ChunkColumnID> TaskQueue;

	std::mutex SchedulerLock;

	std::deque<std::thread> Workers;
	std::deque<std::deque<ChunkColumnID>> WorkerTask;
	std::deque<std::deque<ChunkLightingContainer*>> WorkerOutput;
	std::deque<std::mutex> WorkerLocks;

	std::vector<ChunkLightingContainer*> Output;
	WorldAccess* world;

	std::thread schedulerThread;

	int threadCount = 0;

	bool stop = true;

	std::vector<ChunkLightingContainer*> SkyLighting(ChunkColumnID id) {
		uint8_t DarknessLightLevel = 12;
		std::vector<ChunkLightingContainer*> out = {};
		glm::ivec3 pos = ChunkIDToPOS(id);
		std::vector<int> Heightmap = world->getColumnHeightmap(pos);

		if (Heightmap.size() == 0) {
			return out;
		}

		pos.y *= 32;

		ChunkColumn* col = world->getColumn(pos);
		

		for (int i = 0; i < 32; i++) {
			if (!col->LightDirty[i]) {
				continue;
			}

			ChunkLightingContainer* lighting = new ChunkLightingContainer;
			lighting->ResetLighting();

			int relativeY = i;
			
			col->LightDirty[relativeY] = false;

			//Need to account for spread later
			for (int x = 0; x < 16; x++) {
				for (int z = 0; z < 16; z++) {

					int HeightmapCol = Heightmap[x * 16 + z];

					if (HeightmapCol <= relativeY * 16) {
						continue;
					}

					int ChunkHeightStart = 15;

					if ((HeightmapCol >> 4) == relativeY) {
						ChunkHeightStart = HeightmapCol & 0b1111;
						for (int y = ChunkHeightStart - 1; y >= 0; y--) {
							lighting->EditLight(x, y, z, DarknessLightLevel);
						}
					}
					else {
						for (int y = ChunkHeightStart; y >= 0; y--) {
							lighting->EditLight(x, y, z, DarknessLightLevel);
						}
					}

					
				}
			}

			lighting->Position = glm::ivec3(pos.x, relativeY + pos.y, pos.z);
			out.emplace_back(lighting);
		}
		

		return out;
	}

public:
	void Generate(std::vector<ChunkColumnID> IDs) {
		SchedulerLock.lock();
		TaskQueue.insert(TaskQueue.end(), IDs.begin(), IDs.end());
		SchedulerLock.unlock();
	}

	std::vector<ChunkLightingContainer*> GetOutput() {
		SchedulerLock.lock();
		std::vector<ChunkLightingContainer*> out = Output;
		Output.clear();
		SchedulerLock.unlock();

		return out;
	}

	void Stop() {
		stop = true;
		schedulerThread.join();
		for (size_t i = 0; i < Workers.size(); i++) {
			Workers[i].join();
		}
	}

	void Start(int lightEngineThreadsCount, WorldAccess* w) {
		stop = false;
		threadCount = lightEngineThreadsCount;
		world = w;

		Workers.resize(threadCount);
		WorkerTask.resize(threadCount);
		WorkerLocks.resize(threadCount);
		WorkerOutput.resize(threadCount);

		schedulerThread = std::thread(&LightingEngine::scheduler, this);

		for (int i = 0; i < threadCount; i++) {
			Workers[i] = std::thread(&LightingEngine::worker, this, i);
		}
	}

	void queueChunk(ChunkColumnID columnID) {
		//queue chunk
		SchedulerLock.lock();
		TaskQueue.emplace_back(columnID);
		SchedulerLock.unlock();
	}

	void worker(int id) {
		const int WorkerID = id;

		std::deque<ChunkColumnID> Jobs;

		std::deque<ChunkLightingContainer*> FinishedJobs;

		while (!stop) {
			//Fetches all of the tasks and put it in "Jobs"

			WorkerLocks[WorkerID].lock();
			Jobs.insert(Jobs.end(), WorkerTask[WorkerID].begin(), WorkerTask[WorkerID].end());
			WorkerTask[WorkerID].clear();
			WorkerLocks[WorkerID].unlock();

			//Generates the chunks

			const int NumJobs = Jobs.size();

			int count = 0;
			int BatchSize = 100;

			for (int i = 0; i < NumJobs; i++) {
				ChunkColumnID task = Jobs.front(); //fetches task
				Jobs.pop_front();
				//Generate
				glm::ivec3 ColPos = ChunkIDToPOS(task);

				std::vector<ChunkLightingContainer*> chunks = SkyLighting(task);

				FinishedJobs.insert(FinishedJobs.end(), chunks.begin(), chunks.end());

				count++;
				if (count == BatchSize) {
					break;
				}
			}

			if (FinishedJobs.size() != 0) {
				WorkerLocks[WorkerID].lock();
				WorkerOutput[WorkerID].insert(WorkerOutput[WorkerID].end(), FinishedJobs.begin(), FinishedJobs.end());
				FinishedJobs.clear();
				WorkerLocks[WorkerID].unlock();
			}

			timerSleepNotPrecise(1);
		}

		Jobs.clear();
		Logger.LogInfo("Light Engine", "Shutting down light engine worker: " + std::to_string(WorkerID));
	}

	void scheduler() {
		int WorkerSelection = 0;

		std::deque<std::deque<ChunkColumnID>> DistributedTasks;
		std::deque<std::deque<ChunkLightingContainer*>> ChunkOutputs;

		DistributedTasks.resize(threadCount);
		ChunkOutputs.resize(threadCount);

		std::deque<ChunkColumnID> InternalTaskList;

		while (!stop) {

			//Fetch tasks

			SchedulerLock.lock();
			InternalTaskList = TaskQueue;
			TaskQueue.clear();
			SchedulerLock.unlock();

			//Interally distributes the jobs

			while (!InternalTaskList.empty()) {

				ChunkColumnID task = InternalTaskList.front();
				InternalTaskList.pop_front();

				DistributedTasks[WorkerSelection].push_back(task);

				WorkerSelection++;

				if (WorkerSelection == threadCount)
					WorkerSelection = 0;
			}

			//Distributes the tasks

			for (int i = 0; i < threadCount; i++) {
				WorkerLocks[i].lock();
				WorkerTask[i].insert(WorkerTask[i].end(), DistributedTasks[i].begin(), DistributedTasks[i].end());
				WorkerLocks[i].unlock();
				DistributedTasks[i].clear();
			}

			//Fetches worker output
			for (int i = 0; i < threadCount; i++) {
				WorkerLocks[i].lock();
				ChunkOutputs[i].insert(ChunkOutputs[i].end(), WorkerOutput[i].begin(), WorkerOutput[i].end());
				WorkerOutput[i].clear();
				WorkerLocks[i].unlock();
			}

			//Output the chunks so it can be used

			SchedulerLock.lock();
			for (int i = 0; i < threadCount; i++) {
				Output.insert(Output.end(), ChunkOutputs[i].begin(), ChunkOutputs[i].end());
				ChunkOutputs[i].clear();
			}
			SchedulerLock.unlock();

			timerSleepNotPrecise(1);

		}

		Logger.LogInfo("Light Engine", "Shutting down light engine scheduler");

	}

};
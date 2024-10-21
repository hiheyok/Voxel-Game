#include "Generator.h"
#include "../Chunk/ChunkID.h"
#include "../../Utils/Clock.h"
#include "Generators/GeneratorType.h"
#include <concurrent_unordered_set.h>

using namespace std;
using namespace glm;

vector<Chunk*> ChunkGeneration::GetOutput() {
	SchedulerLock.lock();
	vector<Chunk*> out = Output;
	Output.clear();
	SchedulerLock.unlock();

	return out;
}

void ChunkGeneration::Start(int ThreadCount, long long int WorldSeedIn) {
	WorldSeed = WorldSeedIn;
	WorldGenerator::SetSeed(WorldSeedIn);

	WorkerCount = ThreadCount;

	Workers.resize(ThreadCount);
	WorkerTask.resize(ThreadCount);
	WorkerOutput.resize(ThreadCount);
	WorkerLocks.resize(ThreadCount);

	stop = false;

	for (int i = 0; i < ThreadCount; i++) {
		Workers[i] = thread(&ChunkGeneration::Worker, this, i);
	}

	Scheduler = thread(&ChunkGeneration::TaskScheduler, this);

}

void ChunkGeneration::Stop() {
	stop = true;
	Scheduler.join();
	for (size_t i = 0; i < Workers.size(); i++) {
		Workers[i].join();
	}
}

void ChunkGeneration::Worker(int id) {
	const int WorkerID = id;

	deque<std::pair<ChunkID, WorldGeneratorID>> Jobs;

	deque<Chunk*> FinishedJobs;

	while (!stop) {
		WorldGeneratorID generaterUse = Generators.DEBUG;
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
			std::pair<ChunkID, WorldGeneratorID> task = Jobs.front(); //fetches task
			Jobs.pop_front();
			//Generate
			ivec3 pos = ChunkIDToPOS(task.first);
			generaterUse = task.second;

			if (!Generators.GetGenerator(generaterUse)->useTallChunks) {
				Chunk* chunk = Generators.GetGenerator(generaterUse)->Generate(pos);

				chunk->Position = pos;
				chunk->chunkID = task.first;

				FinishedJobs.push_back(chunk);
			}
			else {
				if (pos.y != 0) {
					TallChunk* chunk = new TallChunk();
					std::vector<Chunk*> chunks = chunk->getCubicChunks();

					FinishedJobs.insert(FinishedJobs.end(), chunks.begin(), chunks.end());
					continue;
				}

				TallChunk* chunk = Generators.GetGenerator(generaterUse)->GenerateTall(pos);
				//Decompose
				std::vector<Chunk*> chunks = chunk->getCubicChunks();

				FinishedJobs.insert(FinishedJobs.end(), chunks.begin(), chunks.end());
			}
			

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
	Logger.LogInfo("World", "Shutting down world gen worker: " + to_string(WorkerID));
}

void ChunkGeneration::TaskScheduler() {

	int WorkerSelection = 0;

	deque<deque<std::pair<ChunkID, WorldGeneratorID>>> DistributedTasks;
	deque<deque<Chunk*>> ChunkOutputs;

	DistributedTasks.resize(WorkerCount);
	ChunkOutputs.resize(WorkerCount);

	vector<std::pair<ChunkID, WorldGeneratorID>> InternalTaskList;

	while (!stop) {
		
		//Fetch tasks

		SchedulerLock.lock();
		InternalTaskList.insert(InternalTaskList.end(), TaskList.begin(), TaskList.end());
		TaskList.clear();
		SchedulerLock.unlock();

		//Interally distributes the jobs

		for (int i = 0; i < InternalTaskList.size(); i++) {
			std::pair<ChunkID, WorldGeneratorID> task = InternalTaskList[i];

			DistributedTasks[i % WorkerCount].push_back(task);
		}
		InternalTaskList.clear();

		//Distributes the tasks

		for (int i = 0; i < WorkerCount; i++) {
			WorkerLocks[i].lock();
			WorkerTask[i].insert(WorkerTask[i].end(),DistributedTasks[i].begin(), DistributedTasks[i].end());
			WorkerLocks[i].unlock();
			DistributedTasks[i].clear();
		}

		//Fetches worker output
		for (int i = 0; i < WorkerCount; i++) {
			WorkerLocks[i].lock();
			ChunkOutputs[i].insert(ChunkOutputs[i].end(), WorkerOutput[i].begin(), WorkerOutput[i].end());
			WorkerOutput[i].clear();
			WorkerLocks[i].unlock();
		}

		//Output the chunks so it can be used

		SchedulerLock.lock();
		for (int i = 0; i < WorkerCount; i++) {
			Output.insert(Output.end(), ChunkOutputs[i].begin(), ChunkOutputs[i].end());
			ChunkOutputs[i].clear();
		}
		SchedulerLock.unlock();
		
		timerSleepNotPrecise(1);

	}

	Logger.LogInfo("World", "Shutting down world gen scheduler");
}

void ChunkGeneration::Generate(ChunkID id, WorldGeneratorID genTypeIn) {
	SchedulerLock.lock();
	TaskList.emplace_back(std::pair<ChunkID, WorldGeneratorID>(id, genTypeIn));
	SchedulerLock.unlock();
}

void ChunkGeneration::Generate(vector<ChunkID> IDs, WorldGeneratorID genTypeIn) {
	SchedulerLock.lock();
	for (const auto& chunkID : IDs) {
		TaskList.emplace_back(std::pair<ChunkID, WorldGeneratorID>(chunkID, genTypeIn));
	}
	SchedulerLock.unlock();
}
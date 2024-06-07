#include "Generator.h"
#include "../Chunk/ChunkID.h"
#include "../../Utils/Clock.h"
#include "Generators/GeneratorType.h"

using namespace std;
using namespace glm;

vector<Chunk*> ChunkGeneration::GetOutput() {
	SchedulerLock.lock();
	vector<Chunk*> out = Output;
	Output.clear();
	SchedulerLock.unlock();

	return out;
}

void ChunkGeneration::Start(int ThreadCount) {
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

	deque<ChunkID> Jobs;

	deque<Chunk*> FinishedJobs;

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
			ChunkID task = Jobs.front(); //fetches task
			Jobs.pop_front();
			//Generate
			ivec3 pos = ChunkIDToPOS(task);

			Chunk* chunk = Generators.GetGenerator(Generators.MOUNTAINS)->Generate(pos);

			chunk->Position = pos;
			chunk->chunkID = task;

			FinishedJobs.push_back(chunk);

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

	deque<deque<ChunkID>> DistributedTasks;
	deque<deque<Chunk*>> ChunkOutputs;

	DistributedTasks.resize(WorkerCount);
	ChunkOutputs.resize(WorkerCount);

	deque<ChunkID> InternalTaskList;

	while (!stop) {
		
		//Fetch tasks

		SchedulerLock.lock();
		InternalTaskList = TaskList;
		TaskList.clear();
		SchedulerLock.unlock();

		//Interally distributes the jobs

		while (!InternalTaskList.empty()) {

			ChunkID task = InternalTaskList.front();
			InternalTaskList.pop_front();

			DistributedTasks[WorkerSelection].push_back(task);
			
			WorkerSelection++;

			if (WorkerSelection == WorkerCount)
				WorkerSelection = 0;
		}

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

void ChunkGeneration::Generate(int x, int y, int z) {
	Generate(getChunkID(x, y, z));
}

void ChunkGeneration::Generate(ChunkID id) {
	SchedulerLock.lock();
	TaskList.push_back(id);
	SchedulerLock.unlock();
}

void ChunkGeneration::Generate(vector<ChunkID> IDs) {
	SchedulerLock.lock();
	TaskList.insert(TaskList.end(), IDs.begin(), IDs.end());
	SchedulerLock.unlock();
}
#include "Generator.h"
#include "../Chunk/ChunkID.h"
#include "../../Utils/Clock.h"

using namespace std;
using namespace glm;

std::deque<Chunk> Generator::GetOutput() {
	SchedulerLock.lock();
	std::deque<Chunk> out = Output;
	Output.clear();
	SchedulerLock.unlock();

	return out;
}

void Generator::Start(int ThreadCount) {
	WorkerCount = ThreadCount;

	Workers.resize(ThreadCount);
	WorkerTask.resize(ThreadCount);
	WorkerOutput.resize(ThreadCount);
	WorkerLocks.resize(ThreadCount);

	stop = false;

	for (int i = 0; i < ThreadCount; i++) {
		Workers[i] = thread(&Generator::Worker, this, i);
	}

	Scheduler = thread(&Generator::TaskScheduler, this);

}

void Generator::Stop() {
	stop = true;

}

void Generator::Worker(int id) {
	const int WorkerID = id;

	deque<ChunkID> Jobs;

	deque<Chunk> FinishedJobs;

	while (!stop) {
		//Fetches all of the tasks and put it in "Jobs"

		WorkerLocks[WorkerID].lock();
		Jobs.insert(Jobs.end(), WorkerTask[WorkerID].begin(), WorkerTask[WorkerID].end());
		WorkerTask[WorkerID].clear();
		WorkerLocks[WorkerID].unlock();

		//Generates the chunks
		while (!Jobs.empty()) {
			ChunkID task = Jobs.front(); //fetches task
			Jobs.pop_front();
			//Generate
			ivec3 pos = ChunkIDToPOS(task);
			
			Chunk NewChunk;
			NewChunk.SetPosition(pos.x, pos.y, pos.z);
			NewChunk.Generate(&noise);
			FinishedJobs.emplace_back(NewChunk);
		}



		WorkerLocks[WorkerID].lock();
		WorkerOutput[WorkerID].insert(WorkerOutput[WorkerID].end(), FinishedJobs.begin(), FinishedJobs.end());
		FinishedJobs.clear();
		WorkerLocks[WorkerID].unlock();

		timerSleepNotPrecise(5);
	}

}

void Generator::TaskScheduler() {

	int WorkerSelection = 0;

	std::deque<std::deque<ChunkID>> DistributedTasks;
	std::deque<std::deque<Chunk>> ChunkOutputs;

	DistributedTasks.resize(WorkerCount);
	ChunkOutputs.resize(WorkerCount);

	std::deque<ChunkID> InternalTaskList;

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

			if (WorkerSelection % WorkerCount == 0)
				WorkerSelection = 0;
		}

		//Distributes the tasks

		for (int i = 0; i < WorkerCount; i++) {
			WorkerLocks[i].lock();
			WorkerTask[i].insert(WorkerTask[i].end(),DistributedTasks[i].begin(), DistributedTasks[i].end());
			WorkerLocks[i].unlock();
			DistributedTasks[i].clear();
		}

		//Manages worker output


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
		
		timerSleepNotPrecise(5);

	}
}

void Generator::Generate(int x, int y, int z) {
	SchedulerLock.lock();
	TaskList.push_back(getChunkID(x, y, z));
	SchedulerLock.unlock();
}
#pragma once

#include "../Chunk/Chunk.h"
#include "../../Utils/FastNoiseLite.h"

#include <deque>
#include <thread>

#include <mutex>

class ChunkGeneration {
public:
	void Start(int ThreadCount);

	void Stop(); //Stop the threads and does some clean up

	void Generate(int x, int y, int z);

	void Generate(ChunkID id);

	void Generate(std::vector<ChunkID> IDs);

	std::vector<Chunk*> GetOutput();

private:

	std::vector<Chunk*> Output; //Once a chunk is generated, the chunk is added here

	void TaskScheduler(); //Manages how jobs gets distributed between the workers and manages the output of the workers

	void Worker(int id);

	int WorkerCount = NULL;

	std::deque<ChunkID> TaskList; //All tasks will go in here and they will be distributed to all the workers

	std::deque<std::thread> Workers;
	std::deque<std::deque<ChunkID>> WorkerTask;
	std::deque<std::deque<Chunk*>> WorkerOutput;
	std::deque<std::mutex> WorkerLocks;

	std::thread Scheduler;
	std::mutex SchedulerLock;

	bool stop = false;
};
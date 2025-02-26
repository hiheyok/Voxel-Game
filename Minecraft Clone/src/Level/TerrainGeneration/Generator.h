#pragma once
#include "../Chunk/Chunk.h"
#include "../../Utils/FastNoiseLite.h"
#include "../TerrainGeneration/Generators/GeneratorType.h"
#include <deque>
#include <thread>
#include <mutex>

class ChunkGeneration {
public:
	void Start(int ThreadCount, long long int WorldSeedIn);

	void Stop(); //Stop the threads and does some clean up

	void Generate(ChunkPos id, WorldGeneratorID genTypeIn);

	void Generate(std::vector<ChunkPos> IDs, WorldGeneratorID genTypeIn);

	std::vector<Chunk*> GetOutput();

private:

	std::vector<Chunk*> Output; //Once a chunk is generated, the chunk is added here

	void TaskScheduler(); //Manages how jobs gets distributed between the workers and manages the output of the workers

	void Worker(int id);

	int WorkerCount = NULL;

	std::vector<std::pair<ChunkPos, WorldGeneratorID>> TaskList; //All tasks will go in here and they will be distributed to all the workers

	std::deque<std::thread> Workers;
	std::deque<std::deque<std::pair<ChunkPos, WorldGeneratorID>>> WorkerTask;
	std::deque<std::deque<Chunk*>> WorkerOutput;
	std::deque<std::mutex> WorkerLocks;

	std::thread Scheduler;
	std::mutex SchedulerLock;

	long long int WorldSeed = 0;

	bool stop = false;
};
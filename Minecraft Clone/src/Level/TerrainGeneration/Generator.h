#pragma once
#include "../Chunk/Chunk.h"
#include "../../Utils/FastNoiseLite.h"
#include "../TerrainGeneration/Generators/GeneratorType.h"
#include <deque>
#include <thread>
#include <mutex>
#include <memory>

class ChunkGeneration {
public:
	void Start(int ThreadCount, long long int WorldSeedIn);

	void Stop(); //Stop the threads and does some clean up

	void Generate(ChunkPos id, WorldGeneratorID genTypeIn);

	void Generate(std::vector<ChunkPos> ids, WorldGeneratorID genTypeIn);

	std::vector<Chunk*> GetOutput();

private:

	std::vector<Chunk*> output_; //Once a chunk is generated, the chunk is added here

	void TaskScheduler(); //Manages how jobs gets distributed between the workers and manages the output of the workers

	void Worker(int id);

	int worker_count_ = NULL;

	std::vector<std::pair<ChunkPos, WorldGeneratorID>> task_list_; //All tasks will go in here and they will be distributed to all the workers

	std::deque<std::thread> workers_;
	std::deque<std::deque<std::pair<ChunkPos, WorldGeneratorID>>> worker_task_;
	std::deque<std::deque<Chunk*>> worker_output_;
	std::deque<std::mutex> worker_locks_;

	std::thread scheduler_;
	std::mutex scheduler_lock_;

	long long int world_seed_ = 0;

	bool stop = false; // TODO: Rename
};
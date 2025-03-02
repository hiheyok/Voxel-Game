#pragma once
#include "../World/WorldDataAccess.h"
#include "../../Utils/Clock.h"
#include <thread>
#include <mutex>
#include <glm/vec4.hpp>
#include "../../Utils/Containers/BitStorage.h"
#include "../../Utils/Containers/FIFOQueue.h"

class LightingEngine {
private:
	std::deque<ChunkPos> task_queue_;

	std::mutex scheduler_lock_;

	std::deque<std::thread> workers_;
	std::deque<std::deque<ChunkPos>> worker_task_;
	std::deque<std::deque<std::shared_ptr<ChunkLightingContainer>>> worker_output_;
	std::deque<std::mutex> worker_locks_;


	std::vector<std::shared_ptr<ChunkLightingContainer>> output_;
	WorldAccess* world_;

	std::thread scheduler_thread_;

	int thread_count_ = 0;
	const size_t DEFAULT_FIFO_QUEUE_SIZE = 32768;

	bool stop_ = true;

	void IncreaseLightLevel(std::shared_ptr<ChunkLightingContainer>& container, uint8_t lvl, int x, int y, int z);

	void LightSpreadSky(Chunk* chunk, std::shared_ptr<ChunkLightingContainer>& container, const Heightmap& heightmap, int chunkHeight, int x, int y, int z);

	void WorkOnChunkSkylight(Chunk* chunk, std::shared_ptr<ChunkLightingContainer>& light, const Heightmap& heightmap, int chunkHeight);

	std::vector<std::shared_ptr<ChunkLightingContainer>> SkyLighting(const ChunkPos& id);

public:
	void Generate(std::vector<ChunkPos> IDs);

	std::vector<std::shared_ptr<ChunkLightingContainer>> GetOutput();

	void Stop();

	void Start(int lightEngineThreadsCount, WorldAccess* w);

	void QueueChunk(const ChunkPos& columnID);

	void Worker(int id);

	void Scheduler();

};
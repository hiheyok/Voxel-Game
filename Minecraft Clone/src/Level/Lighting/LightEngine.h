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
	std::deque<ChunkColumnID> TaskQueue;

	std::mutex SchedulerLock;

	std::deque<std::thread> Workers;
	std::deque<std::deque<ChunkColumnID>> WorkerTask;
	std::deque<std::deque<ChunkLightingContainer*>> WorkerOutput;
	std::deque<std::mutex> WorkerLocks;

	std::vector<FixedFIFOQueue<uint16_t>> FIFOQueues;

	std::vector<ChunkLightingContainer*> Output;
	WorldAccess* world;

	std::thread schedulerThread;

	int threadCount = 0;
	const uint64_t DEFAULT_FIFO_QUEUE_SIZE = 32768;

	bool stop = true;

	void increaseLightLevel(ChunkLightingContainer* container, uint8_t lvl, int x, int y, int z);

	void LightSpreadSky(Chunk* chunk, ChunkLightingContainer* container, std::vector<uint16_t>& heightmap, int ChunkHeight, int x, int y, int z, int workerID);

	void WorkOnChunkSkylight(Chunk* chunk, ChunkLightingContainer* light, std::vector<uint16_t>& heightmap, int ChunkHeight, int workerID);

	std::vector<ChunkLightingContainer*> SkyLighting(ChunkColumnID id, int workerID);

public:
	void Generate(std::vector<ChunkColumnID> IDs);

	std::vector<ChunkLightingContainer*> GetOutput();

	void Stop();

	void Start(int lightEngineThreadsCount, WorldAccess* w);

	void queueChunk(ChunkColumnID columnID);

	void worker(int id);

	void scheduler();

};
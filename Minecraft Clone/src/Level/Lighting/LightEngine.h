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
	std::deque<ChunkPos> TaskQueue;

	std::mutex SchedulerLock;

	std::deque<std::thread> Workers;
	std::deque<std::deque<ChunkPos>> WorkerTask;
	std::deque<std::deque<ChunkLightingContainer*>> WorkerOutput;
	std::deque<std::mutex> WorkerLocks;


	std::vector<ChunkLightingContainer*> Output;
	WorldAccess* world;

	std::thread schedulerThread;

	int threadCount = 0;
	const size_t DEFAULT_FIFO_QUEUE_SIZE = 32768;

	bool stop = true;

	void increaseLightLevel(ChunkLightingContainer* container, uint8_t lvl, int x, int y, int z);

	void LightSpreadSky(Chunk* chunk, ChunkLightingContainer* container, const Heightmap& heightmap, int ChunkHeight, int x, int y, int z);

	void WorkOnChunkSkylight(Chunk* chunk, ChunkLightingContainer* light, const Heightmap& heightmap, int ChunkHeight);

	std::vector<ChunkLightingContainer*> SkyLighting(const ChunkPos& id, int workerID);

public:
	void Generate(std::vector<ChunkPos> IDs);

	std::vector<ChunkLightingContainer*> GetOutput();

	void Stop();

	void Start(int lightEngineThreadsCount, WorldAccess* w);

	void queueChunk(const ChunkPos& columnID);

	void worker(int id);

	void scheduler();

};
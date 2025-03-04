#pragma once
#include "../World/WorldDataAccess.h"
#include "../../Utils/Clock.h"
#include <thread>
#include <mutex>
#include <glm/vec4.hpp>
#include "../../Utils/Containers/BitStorage.h"
#include "../../Utils/Containers/FIFOQueue.h"
#include "../../Utils/ThreadPool.h"

class LightingEngine {
public:
	void Generate(std::vector<ChunkPos> IDs);

	std::vector<std::shared_ptr<ChunkLightingContainer>> GetOutput();

	void Stop();

	void Start(int lightEngineThreadsCount, WorldAccess* w);

	void QueueChunk(const ChunkPos& pos);


	void Scheduler();

private:
	static std::vector<std::shared_ptr<ChunkLightingContainer>> Worker(const ChunkPos& pos);

	std::unique_ptr<ThreadPool<ChunkPos, std::vector<std::shared_ptr<ChunkLightingContainer>>, LightingEngine::Worker>> lighting_thread_pool_;
	static WorldAccess* world_;

	static void IncreaseLightLevel(std::shared_ptr<ChunkLightingContainer>& container, uint8_t lvl, int x, int y, int z);

	static void LightSpreadSky(Chunk* chunk, std::shared_ptr<ChunkLightingContainer>& container, const Heightmap& heightmap, int chunkHeight, int x, int y, int z);

	static void WorkOnChunkSkylight(Chunk* chunk, std::shared_ptr<ChunkLightingContainer>& light, const Heightmap& heightmap, int chunkHeight);

	static std::vector<std::shared_ptr<ChunkLightingContainer>> SkyLighting(const ChunkPos& id);
	const static size_t DEFAULT_FIFO_QUEUE_SIZE = 32768;

};

inline WorldAccess* LightingEngine::world_ = nullptr;
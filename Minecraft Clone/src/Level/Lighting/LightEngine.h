#pragma once
#include <memory>

#include "Utils/ThreadPool.h"
#include "Core/Typenames.h"

class LightStorage;
class Heightmap;
class Chunk;

struct WorldAccess;

class LightingEngine {
public:
    void Generate(std::vector<ChunkPos> IDs);

    std::vector<std::unique_ptr<LightStorage>> GetOutput();

    void Stop();

    void Start(int lightEngineThreadsCount, WorldAccess* w);

    void QueueChunk(const ChunkPos& pos);

    void Scheduler();

    size_t GetQueueSize();

private:
    static std::vector<std::unique_ptr<LightStorage>> Worker(const ChunkPos& pos);

    std::unique_ptr<ThreadPool<ChunkPos, std::vector<std::unique_ptr<LightStorage>>, LightingEngine::Worker>> lighting_thread_pool_;
    static WorldAccess* world_;

    static void IncreaseLightLevel(std::unique_ptr<LightStorage>& container, uint8_t lvl, int x, int y, int z);

    static void LightSpreadSky(Chunk* chunk, std::unique_ptr<LightStorage>& container, const Heightmap& heightmap, int chunkHeight, int x, int y, int z);

    static void WorkOnChunkSkylight(Chunk* chunk, std::unique_ptr<LightStorage>& light, const Heightmap& heightmap, int chunkHeight);

    static std::vector<std::unique_ptr<LightStorage>> SkyLighting(const ChunkPos& id);
    const static size_t DEFAULT_FIFO_QUEUE_SIZE = 32768;
};

inline WorldAccess* LightingEngine::world_ = nullptr;
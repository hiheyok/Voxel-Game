#pragma once
#include <memory>

#include "Core/Typenames.h"
#include "Utils/ThreadPool.h"

class LightStorage;
class Heightmap;
class Chunk;
class WorldInterface;

class LightEngine {
   public:
    void Generate(const std::vector<ChunkPos>& IDs);

    std::vector<std::unique_ptr<LightStorage>> GetOutput();

    void Stop();

    void Start(int lightEngineThreadsCount, WorldInterface* w);

    void QueueChunk(const ChunkPos& pos);

    size_t GetQueueSize();

   private:
    std::unique_ptr<LightStorage> Worker(const ChunkPos& pos);

    std::unique_ptr<ThreadPool<ChunkPos, std::unique_ptr<LightStorage>>>
        lighting_thread_pool_;
    WorldInterface* world_;

    void IncreaseLightLevel(std::unique_ptr<LightStorage>& container,
                            uint8_t lvl, int x, int y, int z);

    void WorkOnChunkSkylight(Chunk* chunk,
                             std::unique_ptr<LightStorage>& light);

    std::unique_ptr<LightStorage> SkyLighting(const ChunkPos& id);
    const static size_t DEFAULT_FIFO_QUEUE_SIZE = 32768;
};
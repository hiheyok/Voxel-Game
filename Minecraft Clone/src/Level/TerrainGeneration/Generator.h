#pragma once
#include <memory>
#include "../Typenames.h"
#include "../../Utils/FastNoiseLite.h"
#include "../../Utils/ThreadPool.h"

class Chunk;

class ChunkGeneration {
public:
    void Start(int ThreadCount, long long int WorldSeedIn);

    void Stop(); //Stop the threads and does some clean up

    void Generate(ChunkPos id, WorldGeneratorID genTypeIn);

    void Generate(std::vector<ChunkPos> ids, WorldGeneratorID genTypeIn);

    std::vector<std::unique_ptr<Chunk>> GetOutput();

private:
    /*
    * Worker submitting output as std::vector<Chunk*> to handle tall chunks with multiple sub chunks for now
    */
    static std::vector<std::unique_ptr<Chunk>> Worker(const std::pair<ChunkPos, WorldGeneratorID>& task);

    std::unique_ptr<ThreadPool<std::pair<ChunkPos, WorldGeneratorID>, std::vector<std::unique_ptr<Chunk>>, ChunkGeneration::Worker>> gen_pool_;

    bool stop_ = false;
    bool collect_ready_ = false;
};
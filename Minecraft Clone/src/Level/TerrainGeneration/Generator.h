#pragma once
#include <memory>
#include "Core/Typenames.h"
#include "Utils/FastNoiseLite.h"
#include "Utils/ThreadPool.h"

class Chunk;

class ChunkGenerator {
public:
    void Start(int ThreadCount, long long int WorldSeedIn, WorldGeneratorID worldGeneratorType);

    void Stop(); //Stop the threads and does some clean up

    void Generate(const std::vector<ChunkPos>& ids);
    void Generate(const ChunkPos& ids);

    std::vector<std::unique_ptr<Chunk>> GetOutput();

private:
    /*
    * Worker submitting output as std::vector<Chunk*> to handle tall chunks with multiple sub chunks for now
    */
    std::vector<std::unique_ptr<Chunk>> Worker(const ChunkPos& task);

    std::unique_ptr<ThreadPool<ChunkPos, std::vector<std::unique_ptr<Chunk>>>> gen_pool_;

    WorldGeneratorID world_generator_type_;

    bool stop_ = false;
    bool collect_ready_ = false;
};
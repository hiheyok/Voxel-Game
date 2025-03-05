#pragma once
#include "../../Chunk/Chunk.h"
#include "../../Chunk/TallChunk.h"
#include <glm/vec3.hpp>

class WorldGenerator {
public:
    WorldGenerator() {

    }

    static void SetSeed(long long worldSeedIn) {
        WorldGenerator::world_seed_ = worldSeedIn;
    }

    virtual Chunk* Generate(const ChunkPos& pos) {
        return nullptr;
    }
    // TODO: Use custom tall chunk positioning
    virtual TallChunk* GenerateTall(const ChunkPos& pos) {
        return nullptr;
    }

    bool use_tall_chunks_ = false;

    static long long world_seed_;
};

inline long long WorldGenerator::world_seed_ = -1587754402;
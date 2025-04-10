#pragma once
#include <glm/vec3.hpp>

#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/TallChunk.h"

// TODO: Fix me later

class WorldGenerator {
public:
    WorldGenerator() {

    }

    static void SetSeed(long long worldSeedIn) {
        WorldGenerator::world_seed_ = worldSeedIn;
    }

    virtual void Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) {
        (void)pos;
        (void)chunk;
        g_logger.LogError("WorldGenerator::Generate", "Don't use");
    }
    // TODO: Use custom tall chunk positioning
    virtual void GenerateTall(const ChunkPos& pos, std::unique_ptr<TallChunk>& chunk) {
        (void)pos;
        (void)chunk;
        g_logger.LogError("WorldGenerator::GenerateTall", "Don't use");
    }

    bool use_tall_chunks_ = false;

    static long long world_seed_;
};

inline long long WorldGenerator::world_seed_ = -1587754402;
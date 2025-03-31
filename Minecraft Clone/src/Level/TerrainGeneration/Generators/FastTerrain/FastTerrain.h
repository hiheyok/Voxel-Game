#pragma once
#include "../Generator.h"
#include <memory>

class FastNoiseLite;

class FastTerrain : public WorldGenerator {
public:
    FastTerrain();
    
    void Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) override;

    std::unique_ptr<FastNoiseLite> noise_;

    static constexpr double TREE_RAND_VAL = 0.5;
    static constexpr double TREE_RAND_VAL_RANGE = .01f;
};
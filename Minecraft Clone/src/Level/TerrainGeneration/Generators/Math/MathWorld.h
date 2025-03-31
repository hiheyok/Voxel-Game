#pragma once
#include "../Generator.h"

class MathWorld : public WorldGenerator {
public:
    MathWorld();

    void Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) override;
};
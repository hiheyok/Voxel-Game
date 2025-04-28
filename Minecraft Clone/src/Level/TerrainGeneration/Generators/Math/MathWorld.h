#pragma once
#include "Level/TerrainGeneration/Generators/Generator.h"

class MathWorld : public WorldGenerator {
   public:
    MathWorld();

    void Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) override;
};
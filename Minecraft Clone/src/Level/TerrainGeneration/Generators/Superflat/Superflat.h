#pragma once
#include "Level/TerrainGeneration/Generators/Generator.h"

class SuperflatWorld : public WorldGenerator {
   public:
    SuperflatWorld();

    void Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) override;
};
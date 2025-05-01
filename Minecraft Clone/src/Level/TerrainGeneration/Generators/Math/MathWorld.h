#pragma once
#include "Level/TerrainGeneration/Generators/Generator.h"

class MathWorld : public WorldGenerator {
 public:
  MathWorld();

  void Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) override;
};

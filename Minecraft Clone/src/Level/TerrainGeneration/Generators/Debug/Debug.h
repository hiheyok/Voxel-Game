#pragma once
#include "Level/TerrainGeneration/Generators/Generator.h"

class DebugWorld : public WorldGenerator {
 public:
  DebugWorld();

  void Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) override;
};

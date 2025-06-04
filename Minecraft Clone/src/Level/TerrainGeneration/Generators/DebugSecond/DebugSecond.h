#pragma once
#include "Level/TerrainGeneration/Generators/Generator.h"

class DebugWorldSecond : public WorldGenerator {
 public:
  explicit DebugWorldSecond(GameContext&);
  ~DebugWorldSecond();
  
  void Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) override;
};

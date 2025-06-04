#pragma once
#include "Level/TerrainGeneration/Generators/Generator.h"

class DebugPositioning : public WorldGenerator {
 public:
  explicit DebugPositioning(GameContext&);
  ~DebugPositioning();

  void Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) override;
};

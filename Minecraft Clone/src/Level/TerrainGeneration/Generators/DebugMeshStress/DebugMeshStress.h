#pragma once
#include "Level/TerrainGeneration/Generators/Generator.h"

// Stress mesh engine with worst case
class DebugMeshStress : public WorldGenerator {
 public:
  explicit DebugMeshStress(GameContext&);
  ~DebugMeshStress();
  
  void Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) override;
};

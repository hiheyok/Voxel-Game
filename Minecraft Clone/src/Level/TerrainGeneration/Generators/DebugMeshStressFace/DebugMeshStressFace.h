#pragma once
#include "Level/TerrainGeneration/Generators/Generator.h"

class DebugMeshStressFace : public WorldGenerator {
 public:
  explicit DebugMeshStressFace(GameContext&);
  ~DebugMeshStressFace();

  void Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) override;
};

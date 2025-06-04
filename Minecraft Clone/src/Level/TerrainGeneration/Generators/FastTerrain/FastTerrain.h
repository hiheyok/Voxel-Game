// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>

#include "Level/TerrainGeneration/Generators/Generator.h"

class FastNoiseLite;

class FastTerrain : public WorldGenerator {
 public:
  explicit FastTerrain(GameContext&);
  ~FastTerrain();

  void Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) override;

  std::unique_ptr<FastNoiseLite> noise_;

  static constexpr double TREE_RAND_VAL = 0.5;
  static constexpr double TREE_RAND_VAL_RANGE = .01f;
};

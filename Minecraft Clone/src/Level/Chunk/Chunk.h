// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <tuple>
#include <vector>

#include "Level/Chunk/ChunkData.h"

class GameContext;

struct ChunkRawData;

enum TerrainType {
  SUPERFLAT,
  FASTTERRAIN,
  MOUNTAINS,
  DEBUG_WORLD,
  MATH_WORLD,
  DEBUG_WORLD_2
};

class Chunk : public ChunkContainer {
 public:
  explicit Chunk(GameContext&);
  Chunk(GameContext&, const ChunkRawData&);
  ~Chunk();
  void UpdateGen();
};

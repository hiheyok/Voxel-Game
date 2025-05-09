// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Chunk/ChunkData.h"

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
  Chunk();
  Chunk(const ChunkRawData&);
  ~Chunk();
  void UpdateGen();
};

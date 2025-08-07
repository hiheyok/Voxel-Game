// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Chunk/Chunk.h"

struct TallChunk {
  std::vector<std::unique_ptr<Chunk>> chunk_sub_column_;

  explicit TallChunk(GameContext&);
  ~TallChunk();

  void SetPosition(int x, int y, int z);
  int GetYPosition();
  int GetXPosition();
  int GetZPosition();
  void SetBlockUnsafe(int x, int y, int z, BlockID block);
  BlockID GetBlockUnsafe(int x, int y, int z);

  GameContext& context_;
};

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Core/Typenames.h"
#include "Level/Block/Block.h"

struct DirtBlock : Block {
  explicit DirtBlock(GameContext&);
  void Tick(BlockPos pos, Dimension* currentWorld) override;
};

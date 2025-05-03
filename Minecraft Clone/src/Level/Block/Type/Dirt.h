// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Core/Typenames.h"
#include "Level/Block/Block.h"

class Dimension;

struct DirtBlock : Block {
  DirtBlock();
  void Tick(BlockPos pos, Dimension* currentWorld) override;
};

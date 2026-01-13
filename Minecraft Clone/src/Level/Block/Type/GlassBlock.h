// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Core/Typenames.h"
#include "Level/Block/Block.h"

struct GlassBlock : Block {
  explicit GlassBlock(GameContext&);
  virtual void Tick(BlockPos pos, Dimension* current_world) override;
};

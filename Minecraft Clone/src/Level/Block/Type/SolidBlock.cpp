// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/SolidBlock.h"
#include "Level/Block/Block.h"
#include "Core/Position/PositionTypes.h"

SolidBlock::SolidBlock(GameContext& context) : Block{context} {}

void SolidBlock::Tick(BlockPos pos, Dimension* current_world) {
  (void)pos;
  (void)current_world;
}

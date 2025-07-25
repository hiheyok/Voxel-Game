// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/SolidBlock.h"

SolidBlock::SolidBlock(GameContext& game_context) : Block{game_context} {
  properties_->light_pass_ = false;
}

void SolidBlock::Tick(BlockPos pos, Dimension* current_world) {
  (void)pos;
  (void)current_world;
}

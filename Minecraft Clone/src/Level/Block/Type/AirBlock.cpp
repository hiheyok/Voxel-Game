// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/AirBlock.h"

AirBlock::AirBlock(GameContext& game_context) : Block{game_context} {
  properties_->is_solid_ = false;
  properties_->transparency_ = true;
  properties_->is_fluid_ = false;
  properties_->light_pass_ = true;
  properties_->opacity_ = 0;
}

void AirBlock::Tick(BlockPos pos, Dimension* current_world) {
  (void)pos;
  (void)current_world;
}

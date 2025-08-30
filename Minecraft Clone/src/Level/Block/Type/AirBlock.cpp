// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/AirBlock.h"

AirBlock::AirBlock(GameContext& context) : Block{context} {
  properties_->is_solid_ = false;
  properties_->transparency_ = true;
  properties_->is_fluid_ = false;
  properties_->opacity_ = 0;
}

void AirBlock::Tick(BlockPos pos, Dimension* current_world) {
  (void)pos;
  (void)current_world;
}

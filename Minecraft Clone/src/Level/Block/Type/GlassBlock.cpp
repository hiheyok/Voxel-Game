// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/GlassBlock.h"

GlassBlock::GlassBlock(GameContext& context) : Block{context} {
  properties_->is_solid_ = true;
  properties_->is_fluid_ = false;
  properties_->transparency_ = true;
  properties_->opacity_ = 0;
  properties_->light_emission = 0;
}

void GlassBlock::Tick(BlockPos pos, Dimension* current_world) {}

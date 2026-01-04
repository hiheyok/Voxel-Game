// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/DefaultBlock.h"

#include "Core/Position/PositionTypes.h"
#include "Level/Block/Block.h"
#include "Level/Dimension/Dimension.h"

DefaultBlock::DefaultBlock(GameContext& context) : Block{context} {
  properties_->is_solid_ = true;
  properties_->transparency_ = false;
  properties_->is_fluid_ = false;
}

void DefaultBlock::Tick(BlockPos pos, Dimension* currentWorld) {
  (void)currentWorld;
  (void)pos;
}

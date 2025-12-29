// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/PlantBlock.h"

PlantBlock::PlantBlock(GameContext& context) : Block{context} {
  properties_->opacity_ = 0;
  properties_->is_solid_ = true;  // TODO(hiheyok): tmp fix
  // Add different collision patterns later
}

void PlantBlock::Tick(BlockPos pos, Dimension* current_world) {
  (void)pos;
  (void)current_world;
}

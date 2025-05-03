// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/GravityBlock.h"

#include "Level/Dimension/Dimension.h"
#include "Level/Entity/Entities.h"
#include "Level/Event/EventHandler.h"

GravityBlock::GravityBlock() {
  properties_->is_solid_ = true;
  properties_->transparency_ = false;
  properties_->is_fluid_ = false;
  properties_->light_pass_ = false;
}

void GravityBlock::Tick(BlockPos pos, Dimension* currentWorld) {
  BlockPos belowPos = pos;
  belowPos.y -= 1;
  bool isBlockSupported =
      g_blocks.GetBlockType(currentWorld->world_->GetBlock(belowPos))
          ->properties_->is_solid_;
  if (!isBlockSupported) {
    BlockEvent destroyBlock{pos, g_blocks.AIR, g_event_handler.BlockPlace};
    EntityEvent summonSand;

    summonSand.id_ = g_event_handler.SummonEntity;
    summonSand.pos_ = pos;
    summonSand.entity_type_ = g_entity_list.SAND_GRAVITY_BLOCK;

    currentWorld->event_manager_.AddEvent(destroyBlock);
    currentWorld->event_manager_.AddEvent(summonSand);
  }
}

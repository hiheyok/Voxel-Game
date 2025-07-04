// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/GravityBlock.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Entity/Entities.h"
#include "Level/Event/EventHandler.h"

GravityBlock::GravityBlock(GameContext& game_context) : Block{game_context} {
  properties_->is_solid_ = true;
  properties_->transparency_ = false;
  properties_->is_fluid_ = false;
  properties_->light_pass_ = false;
}

void GravityBlock::Tick(BlockPos pos, Dimension* currentWorld) {
  BlockPos belowPos = pos;
  belowPos.y -= 1;
  bool isBlockSupported =
      game_context_.blocks_
          ->GetBlockType(currentWorld->world_->GetBlock(belowPos))
          ->properties_->is_solid_;
  if (!isBlockSupported) {
    BlockEvent destroyBlock{pos, game_context_.blocks_->AIR,
                            game_context_.event_handler_->BlockPlace};
    EntityEvent summonSand;

    summonSand.id_ = game_context_.event_handler_->SummonEntity;
    summonSand.pos_ = pos;
    summonSand.entity_type_ = game_context_.entities_list_->SAND_GRAVITY_BLOCK;

    currentWorld->event_manager_.AddEvent(destroyBlock);
    currentWorld->event_manager_.AddEvent(summonSand);
  }
}

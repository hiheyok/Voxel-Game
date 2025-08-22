// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/Fluid.h"

#include "Core/GameContext/GameContext.h"
#include "FileManager/Files.h"
#include "Level/Block/Blocks.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Event/EventHandler.h"

Fluid::Fluid(GameContext& context, int spread_rate) : Block{context} {
  properties_->is_solid_ = false;
  properties_->is_fluid_ = true;
  properties_->light_pass_ = false;

  fluid_properties_.spread_rate_ = spread_rate;
}

void Fluid::Tick(BlockPos pos, Dimension* currentWorld) {
  BlockID blockType = currentWorld->world_->GetBlock(pos);

  for (auto offset : Directions<BlockPos>()) {
    BlockPos newPos = pos + offset;

    if (offset == Directions<BlockPos>::kUp) {
      continue;
    }

    BlockID block = currentWorld->world_->GetBlock(newPos);

    if (block != context_.blocks_->AIR) {
      continue;
    }

    if (block == context_.blocks_->WATER) {
      continue;
    }

    if (currentWorld->CheckTickUsed(context_.event_handler_->BlockPlace, pos)) {
      return;
    }

    currentWorld->TickUsed(context_.event_handler_->BlockPlace, pos);
    BlockEvent blockEvent{newPos, blockType,
                          context_.event_handler_->BlockPlace};
    currentWorld->event_manager_.AddEvent(blockEvent);
  }
}

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Event/BlockEventHandles/BlockHandles.h"

#include "Core/GameContext/GameContext.h"
#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Block/Block.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Event/EventHandler.h"
#include "Level/World/WorldInterface.h"
#include "Level/World/WorldUpdater.h"

void UpdateSurrounding(const BlockEvent& blockEvent, Dimension* dimension) {
  for (auto offset : Directions<BlockPos>()) {
    BlockPos new_pos = blockEvent.pos_ + offset;
    BlockID block = dimension->world_->GetBlock(new_pos);

    BlockEvent tickNeighbor{new_pos, block,
                            dimension->context_.event_handler_->BlockTick};
    dimension->event_manager_.AddEvent(tickNeighbor);
  }

  BlockEvent tickMain{blockEvent.pos_,
                      dimension->world_->GetBlock(blockEvent.pos_),
                      dimension->context_.event_handler_->BlockTick};
  dimension->event_manager_.AddEvent(tickMain);
}

void HandlePlaceBlock(const BlockEvent& blockEvent, Dimension* dimension) {
  dimension->world_updater_->SetBlock(blockEvent.block_, blockEvent.pos_);
  UpdateSurrounding(blockEvent, dimension);
}

void HandleBlockTick(const BlockEvent& blockEvent, Dimension* dimension) {
  Block* b = dimension->context_.blocks_->GetBlockType(blockEvent.block_);
  b->Tick(blockEvent.pos_, dimension);
}

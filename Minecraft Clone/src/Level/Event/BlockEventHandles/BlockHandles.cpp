#include "Level/Event/BlockEventHandles/BlockHandles.h"
#include "Level/Event/EventHandler.h"
#include "Level/Block/Block.h"
#include "Level/Block/Type/Dirt.h"
#include "Level/Dimension/Dimension.h"
#include "Level/World/WorldUpdater.h"
#include "Level/World/WorldInterface.h"

void UpdateSurrounding(const BlockEvent& blockEvent, Dimension* dimension) {
    for (const auto& offset : Directions()) {
        BlockPos newPos = blockEvent.pos_ + offset;
        BlockID block = dimension->world_->GetBlock(newPos);

        BlockEvent tickNeighbor{ newPos, block, g_event_handler.BlockTick };
        dimension->event_manager_.AddEvent(tickNeighbor);
    }

    BlockEvent tickMain{ blockEvent.pos_, dimension->world_->GetBlock(blockEvent.pos_), g_event_handler.BlockTick};
    dimension->event_manager_.AddEvent(tickMain);
}

void HandlePlaceBlock(const BlockEvent& blockEvent, Dimension* dimension) {
    dimension->world_updater_->SetBlock(blockEvent.block_, blockEvent.pos_);
    UpdateSurrounding(blockEvent, dimension);
}

void HandleBlockTick(const BlockEvent& blockEvent, Dimension* dimension) {
    Block* b = g_blocks.GetBlockType(blockEvent.block_);
    b->Tick(blockEvent.pos_, dimension);
}
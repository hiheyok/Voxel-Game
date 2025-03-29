#include "BlockHandles.h"
#include "../EventHandler.h"
#include "../../Chunk/Block/Type/Dirt.h"
#include "../../../Level/Dimension/Dimension.h"

void UpdateSurrounding(const Event::BlockEvent& blockEvent, Dimension* dimension) {
    for (int side = 0; side < 6;  side++) {
        BlockPos newPos = blockEvent.pos_;
        newPos.incrementSide(side, 1);

        BlockID block = dimension->world_interactions_.GetBlock(newPos);

        Event::BlockEvent tickNeighbor{newPos, block, g_event_handler.BlockTick};
        dimension->event_manager_.AddEvent(tickNeighbor);
    }

    Event::BlockEvent tickMain{ blockEvent.pos_, dimension->world_interactions_.GetBlock(blockEvent.pos_), g_event_handler.BlockTick};
    dimension->event_manager_.AddEvent(tickMain);
}

void HandlePlaceBlock(const Event::BlockEvent& blockEvent, Dimension* dimension) {
    dimension->world_interactions_.SetBlock(blockEvent.block_, blockEvent.pos_);
    UpdateSurrounding(blockEvent, dimension);
}

void HandleBlockTick(const Event::BlockEvent& blockEvent, Dimension* dimension) {
    Block* b = g_blocks.GetBlockType(blockEvent.block_);
    b->Tick(blockEvent.pos_, dimension);
}
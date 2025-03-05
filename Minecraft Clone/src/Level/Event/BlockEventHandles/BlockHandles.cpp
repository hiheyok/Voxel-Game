#include "BlockHandles.h"
#include "../EventHandler.h"
#include "../../Chunk/Block/Type/Dirt.h"
#include "../../../Level/Dimension/Dimension.h"

void UpdateSurrounding(const BlockPos& pos) {
    Dimension* currentWorld = (Dimension*)Block::dimension_ptr_;
    
    for (int side = 0; side < 6;  side++) {
        BlockPos newPos = pos;
        newPos.incrementSide(side, 1);

        BlockID block = currentWorld->world_interactions_.GetBlock(newPos);

        Event::BlockEvent tickNeighbor{newPos, block, g_event_handler.BlockTick};
        currentWorld->event_manager_.AddEvent(tickNeighbor);
    }

    Event::BlockEvent tickMain{pos, currentWorld->world_interactions_.GetBlock(pos), g_event_handler.BlockTick};
    currentWorld->event_manager_.AddEvent(tickMain);
}

void HandlePlaceBlock(BlockID block, const BlockPos& pos) {
    Dimension* currentWorld = (Dimension*)Block::dimension_ptr_;
    currentWorld->world_interactions_.SetBlock(block, pos);
    UpdateSurrounding(pos);
}

void HandleBlockTick(BlockID block, const BlockPos& pos) {
    Block* b = g_blocks.GetBlockType(block);
    b->Tick(pos);
}
#include "BlockHandles.h"
#include "../EventHandler.h"
#include "../../Chunk/Block/Type/Dirt.h"
#include "../../../Level/Dimension/Dimension.h"

void UpdateSurrounding(const BlockPos& pos) {
	Dimension* CurrentWorld = (Dimension*)Block::DimensionPTR;
	
	for (int side = 0; side < 6;  side++) {
		BlockPos newPos = pos;
		newPos.incrementSide(side, 1);

		BlockID block = CurrentWorld->worldInteractions.getBlock(newPos);

		Event event;
		event.Type = BLOCK_EVENT;
		event.Data.BlockEvent.id = EventHandler.BlockTick;
		event.Data.BlockEvent.pos = newPos;
		event.Data.BlockEvent.block = block;
		CurrentWorld->EventManager.AddEvent(event);
	}

	Event event;
	event.Type = BLOCK_EVENT;
	event.Data.BlockEvent.id = EventHandler.BlockTick;
	event.Data.BlockEvent.pos = pos;
	event.Data.BlockEvent.block = CurrentWorld->worldInteractions.getBlock(pos);
	CurrentWorld->EventManager.AddEvent(event);
}

void HandlePlaceBlock(BlockID block, const BlockPos& pos) {
	Dimension* CurrentWorld = (Dimension*)Block::DimensionPTR;
	CurrentWorld->worldInteractions.setBlock(block, pos);
	UpdateSurrounding(pos);
}

void HandleBlockTick(BlockID block, const BlockPos& pos) {
	Block* b = Blocks.getBlockType(block);
	b->tick(pos);
}
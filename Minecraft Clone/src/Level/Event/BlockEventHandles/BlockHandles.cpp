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

		Event::BlockEvent tickNeighbor{newPos, block, EventHandler.BlockTick};
		CurrentWorld->EventManager.AddEvent(tickNeighbor);
	}

	Event::BlockEvent tickMain{pos, CurrentWorld->worldInteractions.getBlock(pos), EventHandler.BlockTick};
	CurrentWorld->EventManager.AddEvent(tickMain);
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
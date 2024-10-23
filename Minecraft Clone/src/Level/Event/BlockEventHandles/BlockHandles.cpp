#include "BlockHandles.h"
#include "../EventHandler.h"
#include "../../Chunk/Block/Type/Dirt.h"
#include "../../../Level/Dimension/Dimension.h"

void UpdateSurrounding(int x, int y, int z) {
	Dimension* CurrentWorld = (Dimension*)Block::DimensionPTR;

	for (int side = 0; side < 6;  side++) {
		int pos[3]{ x,y,z };

		pos[side >> 1] += (side & 0b1) * 2 - 1;

		BlockID block = CurrentWorld->worldInteractions.getBlock(pos[0], pos[1], pos[2]);

		Event event;
		event.Type = BLOCK_EVENT;
		event.Data.BlockEvent.id = EventHandler.BlockTick;
		event.Data.BlockEvent.x = pos[0];
		event.Data.BlockEvent.y = pos[1];
		event.Data.BlockEvent.z = pos[2];
		event.Data.BlockEvent.block = block;
		CurrentWorld->EventManager.AddEvent(event);
	}

	Event event;
	event.Type = BLOCK_EVENT;
	event.Data.BlockEvent.id = EventHandler.BlockTick;
	event.Data.BlockEvent.x = x;
	event.Data.BlockEvent.y = y;
	event.Data.BlockEvent.z = z;
	event.Data.BlockEvent.block = CurrentWorld->worldInteractions.getBlock(x, y, z);
	CurrentWorld->EventManager.AddEvent(event);
}

void HandlePlaceBlock(BlockID block, int x, int y, int z) {
	Dimension* CurrentWorld = (Dimension*)Block::DimensionPTR;
	CurrentWorld->worldInteractions.setBlock(block, x, y, z);
	UpdateSurrounding(x, y, z);
}

void HandleBlockTick(BlockID block, int x, int y, int z) {
	Block* b = Blocks.getBlockType(block);
	b->tick(x, y, z);
}
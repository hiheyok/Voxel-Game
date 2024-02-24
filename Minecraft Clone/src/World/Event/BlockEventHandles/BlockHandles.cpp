#include "BlockHandles.h"
#include "../EventHandler.h"
#include "../../World.h"
#include "../../Chunk/Block/Type/Dirt.h"



void UpdateSurrounding(int x, int y, int z) {

	World* CurrentWorld = (World*)Block::WorldPTR;

	for (int side = 0; side < 6;  side++) {
		int pos[3]{ x,y,z };

		pos[side >> 1] += (side & 0b1) * 2 - 1;

		BlockID block = CurrentWorld->GetBlock(pos[0], pos[1], pos[2]);

		Event event;
		event.Type = BLOCK_EVENT;
		event.Data.BlockEvent.id = EventHandler.BlockTick;
		event.Data.BlockEvent.x = pos[0];
		event.Data.BlockEvent.y = pos[1];
		event.Data.BlockEvent.z = pos[2];
		event.Data.BlockEvent.block = block;
		CurrentWorld->QueueEvent(event);

	}

	Event event;
	event.Type = BLOCK_EVENT;
	event.Data.BlockEvent.id = EventHandler.BlockTick;
	event.Data.BlockEvent.x = x;
	event.Data.BlockEvent.y = y;
	event.Data.BlockEvent.z = z;
	event.Data.BlockEvent.block = CurrentWorld->GetBlock(x, y, z);
	CurrentWorld->QueueEvent(event);
}

void HandlePlaceBlock(BlockID block, int x, int y, int z) {
	World* CurrentWorld = (World*)Block::WorldPTR;
	CurrentWorld->SetBlock(block, x, y, z);
	UpdateSurrounding(x, y, z);
}

void HandleDirtTick(BlockID block, int x, int y, int z) {
	World* CurrentWorld = (World*)Block::WorldPTR;


	Block* b = Blocks.getBlockType(block);
	b->tick(x, y, z);
}

void HandleBlockTick(BlockID block, int x, int y, int z) {
	World* CurrentWorld = (World*)Block::WorldPTR;

	Block* b = Blocks.getBlockType(block);
	b->tick(x, y, z);
}
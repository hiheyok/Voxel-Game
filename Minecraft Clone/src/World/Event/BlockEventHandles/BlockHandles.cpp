#include "BlockHandles.h"
#include "../EventHandler.h"
#include "../../World.h"
#include "../../Chunk/Block/Type/Dirt.h"



void UpdateSurrounding(int x, int y, int z) {

	World* CurrentWorld = (World*)Block::WorldPTR;

	for (int x2 = -1; x2 <= 1; x2++) {
		for (int z2 = -1; z2 <= 1; z2++) {
			if (CurrentWorld->GetBlock(x + x2, y, z + z2) == Blocks.DIRT) {
				if (CurrentWorld->GetBlock(x + x2, y + 1, z + z2) == Blocks.AIR) {
					Event event;
					event.Type = BLOCK_EVENT;
					event.Data.BlockEvent.id = EventHandler.DirtTick;
					event.Data.BlockEvent.x = x + x2;
					event.Data.BlockEvent.y = y;
					event.Data.BlockEvent.z = z + z2;
					event.Data.BlockEvent.block = Blocks.DIRT;
					CurrentWorld->QueueEvent(event);
					//std::cout << "Dirt Tick Insert\n";
				}
			}
		}
	}
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
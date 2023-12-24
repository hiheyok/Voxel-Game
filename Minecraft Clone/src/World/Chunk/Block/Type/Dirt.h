#pragma once
#include "../Block.h"
#include "../../../World.h"
#include <cstdlib> 
#include "../../../Event/EventHandler.h"

struct DIRT_BLOCK : Block {
	void tick(int x, int y, int z) override {
		bool isGrassNearby = false;

		World* CurrentWorld = (World*)Block::WorldPTR;

		for (int x2 = -1; x2 <= 1; x2++) {
			for (int z2 = -1; z2 <= 1; z2++) {
				for (int y2 = -1; y2 <= 1; y2++) {
					if (CurrentWorld->GetBlock(x + x2, y + y2, z + z2) == Blocks.GRASS) {
						isGrassNearby = true;
						//	goto skip;
					}
				}
				
			}
		}
	//	skip:

		if (!isGrassNearby) {
			return;
		}


		float GRASS_GROW_CHANCE = 0.005;

		int rng = rand() & 0xFFFF;
		if ((float)rng > ((float)0xFFFF) * GRASS_GROW_CHANCE) {
			Event event;
			event.Type = BLOCK_EVENT;
			event.Data.BlockEvent.id = EventHandler.DirtTick;
			event.Data.BlockEvent.x = x;
			event.Data.BlockEvent.y = y;
			event.Data.BlockEvent.z = z;
			event.Data.BlockEvent.block = Blocks.DIRT;
			CurrentWorld->QueueEvent(event);
			return;
		}

		CurrentWorld->SetBlock(Blocks.GRASS, x, y, z);

		for (int x2 = -1; x2 <= 1; x2++) {
			for (int z2 = -1; z2 <= 1; z2++) {
				for (int y2 = -1; y2 <= 1; y2++) {
					if (CurrentWorld->GetBlock(x + x2, y + y2, z + z2) == Blocks.DIRT) {
						if (CurrentWorld->GetBlock(x + x2, y + y2 + 1, z + z2) == Blocks.AIR) {
							Event event;
							event.Type = BLOCK_EVENT;
							event.Data.BlockEvent.id = EventHandler.DirtTick;
							event.Data.BlockEvent.x = x + x2;
							event.Data.BlockEvent.y = y + y2;
							event.Data.BlockEvent.z = z + z2;
							event.Data.BlockEvent.block = Blocks.DIRT;
							CurrentWorld->QueueEvent(event);

						}
					}

				}
				
			}
		}

	}
};
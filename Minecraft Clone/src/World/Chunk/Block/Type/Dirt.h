#pragma once
#include "../Block.h"
#include "../../../World.h"
#include "../../../../Utils/Math/Probability/Probability.h"
#include "../../../Event/EventHandler.h"

struct DirtBlock : Block {

	void tick(int x, int y, int z) override {
		bool isGrassNearby = false;
		World* CurrentWorld = (World*)Block::WorldPTR;

		//Checks if there is space above the block
		if (CurrentWorld->GetBlock(x, y + 1, z) != Blocks.AIR) { //Change to check for transparency later
			return;
		}

		for (int x2 = -1; x2 <= 1; x2++) {
			for (int z2 = -1; z2 <= 1; z2++) {
				for (int y2 = -1; y2 <= 1; y2++) {
					if (x2 == 0 && y2 != 0 && z2 == 0) {
						continue;
					}

					if (CurrentWorld->GetBlock(x + x2, y + y2, z + z2) == Blocks.GRASS) {
						isGrassNearby = true;
						goto skip;
					}
				}
				
			}
		}


		if (!isGrassNearby) {
			return;
		}

		skip:


		float GRASS_GROW_CHANCE = 0.001f; //0.1% Per Tick

		if (TestProbability(1 - GRASS_GROW_CHANCE)) { //Probability of it not happening
			Event event;
			event.Type = BLOCK_EVENT;
			event.Data.BlockEvent.id = EventHandler.BlockTick;
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
							event.Data.BlockEvent.id = EventHandler.BlockTick;
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
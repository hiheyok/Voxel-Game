#pragma once
#include "../../../Event/EventSystem.h"
#include "../Block.h"
#include "../../../../Utils/Math/Probability/Probability.h"
#include "../../../Event/EventHandler.h"

struct GrassProperties {
	double SpreadChance = 0.001;
	double BreakChance = 0.001;
};

struct GrassBlock : Block {

	GrassProperties Properties;

	void tick(int x, int y, int z) override {
	//	std::cout << "Tick\n";


		Dimension* CurrentWorld = static_cast<Dimension*>(Block::DimensionPTR);

		//Checks if ticking block changes 
		if (CurrentWorld->worldInteractions.getBlock(x, y, z) != Blocks.GRASS) {
			return;
		}

		bool BlockOnTopOfGrass = (CurrentWorld->worldInteractions.getBlock(x, y + 1, z) != Blocks.AIR);

		bool isGrassDestroyed = false;

		if (BlockOnTopOfGrass) {
			isGrassDestroyed = GrassDestroyTick(CurrentWorld, x, y, z);
		}

		//If grass destroyed tick ends
		if (isGrassDestroyed) {
			return;
		}

		bool OnlySurroundedByGrass = GrassSpreadTick(CurrentWorld, x, y, z);

		if (OnlySurroundedByGrass && (!BlockOnTopOfGrass)) {
			return;
		}

		Event event;
		event.Type = BLOCK_EVENT;
		event.Data.BlockEvent.id = EventHandler.BlockTick;
		event.Data.BlockEvent.x = x;
		event.Data.BlockEvent.y = y;
		event.Data.BlockEvent.z = z;
		event.Data.BlockEvent.block = Blocks.GRASS;
		CurrentWorld->EventManager.AddEvent(event);
	}

	bool GrassDestroyTick(Dimension* CurrentWorld, int x, int y, int z) {
		//Chance it -doesn't break-
		if (TestProbability(1 - Properties.BreakChance)) {
			return false;
		}

		CurrentWorld->worldInteractions.setBlock(Blocks.DIRT, x, y, z);

		return true;
	}

	//return true if there is no exposed dirt blocks surrounding it
	bool GrassSpreadTick(Dimension* CurrentWorld, int x, int y, int z) {
		bool DirtExposed = false;
		
		for (int x1 = -1; x1 <= 1; x1++) {
			for (int z1 = -1; z1 <= 1; z1++) {
				if (x1 == 0 && z1 == 0) {
					continue;
				}

				for (int y1 = -1; y1 <= 1; y1++) {

					int x2 = x1 + x;
					int y2 = y1 + y;
					int z2 = z1 + z;

					//Checks if block is dirt
					if (CurrentWorld->worldInteractions.getBlock(x2, y2, z2) != Blocks.DIRT) {
						continue;
					}

					//Checks if there isnt any block above
					if (CurrentWorld->worldInteractions.getBlock(x2, y2 + 1, z2) != Blocks.AIR) {
						continue;
					}

					//Chance it spread
					if (TestProbability(Properties.SpreadChance)) {

						Event event;
						event.Type = BLOCK_EVENT;
						event.Data.BlockEvent.id = EventHandler.BlockPlace;
						event.Data.BlockEvent.x = x2;
						event.Data.BlockEvent.y = y2;
						event.Data.BlockEvent.z = z2;
						event.Data.BlockEvent.block = Blocks.GRASS;
						CurrentWorld->EventManager.AddEvent(event);

						continue;
					}

					DirtExposed = true;
				}
			}
		}

		return !DirtExposed;
	}
};
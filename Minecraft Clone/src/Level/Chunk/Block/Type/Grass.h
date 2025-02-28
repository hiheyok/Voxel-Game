#pragma once
#include "../../../Event/EventSystem.h"
#include "../../../../Utils/Math/Probability/Probability.h"
#include "../../../Event/EventHandler.h"

class Block;

struct GrassProperties {
	double SpreadChance = 0.001;
	double BreakChance = 0.001;
};

struct GrassBlock : Block {

	GrassProperties Properties;

	void tick(const BlockPos& pos) override {
		Dimension* CurrentWorld = static_cast<Dimension*>(Block::DimensionPTR);

		//Checks if ticking block changes 
		if (CurrentWorld->worldInteractions.getBlock(pos) != Blocks.GRASS) {
			return;
		}

		BlockPos newPos = pos;
		newPos.y += 1;

		bool BlockOnTopOfGrass = (CurrentWorld->worldInteractions.getBlock(newPos) != Blocks.AIR);

		bool isGrassDestroyed = false;

		if (BlockOnTopOfGrass) {
			isGrassDestroyed = GrassDestroyTick(CurrentWorld, pos);
		}

		//If grass destroyed tick ends
		if (isGrassDestroyed) {
			return;
		}

		bool OnlySurroundedByGrass = GrassSpreadTick(CurrentWorld, pos);

		if (OnlySurroundedByGrass && (!BlockOnTopOfGrass)) {
			return;
		}

		Event::BlockEvent grassSpread{pos, Blocks.GRASS, EventHandler.BlockTick};
		CurrentWorld->EventManager.AddEvent(grassSpread);
	}

	bool GrassDestroyTick(Dimension* CurrentWorld, const BlockPos& pos) {
		//Chance it -doesn't break-
		if (TestProbability(1 - Properties.BreakChance)) {
			return false;
		}

		CurrentWorld->worldInteractions.setBlock(Blocks.DIRT, pos);

		return true;
	}

	//return true if there is no exposed dirt blocks surrounding it
	// TODO: refactor code
	bool GrassSpreadTick(Dimension* CurrentWorld, const BlockPos& pos) {
		bool DirtExposed = false;

		for (int x1 = -1; x1 <= 1; x1++) {
			for (int z1 = -1; z1 <= 1; z1++) {
				if (x1 == 0 && z1 == 0) {
					continue;
				}

				for (int y1 = -1; y1 <= 1; y1++) {

					BlockPos newPos = pos;
					newPos.x += x1;
					newPos.y += y1;
					newPos.z += z1;

					//Checks if block is dirt
					if (CurrentWorld->worldInteractions.getBlock(newPos) != Blocks.DIRT) {
						continue;
					}

					//Checks if there isnt any block above
					newPos.y += 1;
					if (CurrentWorld->worldInteractions.getBlock(newPos) != Blocks.AIR) {
						continue;
					}
					newPos.y -= 1;

					//Chance it spread
					if (TestProbability(Properties.SpreadChance)) {

						Event::BlockEvent blockEvent{newPos,Blocks.GRASS, EventHandler.BlockPlace};
						CurrentWorld->EventManager.AddEvent(blockEvent);

						continue;
					}

					DirtExposed = true;
				}
			}
		}

		return !DirtExposed;
	}
};
#pragma once
#include "../../../Event/EventSystem.h"
#include "../../../../Utils/Math/Probability/Probability.h"
#include "../../../Event/EventHandler.h"

class Block;

struct GrassProperties {
	double spread_chance_ = 0.001;
	double break_chance_ = 0.001;
};

struct GrassBlock : Block {

	GrassProperties properties_;

	void Tick(const BlockPos& pos) override {
		Dimension* currentWorld = static_cast<Dimension*>(Block::dimension_ptr_);

		//Checks if ticking block changes 
		if (currentWorld->world_interactions_.GetBlock(pos) != g_blocks.GRASS) {
			return;
		}

		BlockPos newPos = pos;
		newPos.y += 1;

		bool blockOnTopOfGrass = (currentWorld->world_interactions_.GetBlock(newPos) != g_blocks.AIR);

		bool isGrassDestroyed = false;

		if (blockOnTopOfGrass) {
			isGrassDestroyed = GrassDestroyTick(currentWorld, pos);
		}

		//If grass destroyed tick ends
		if (isGrassDestroyed) {
			return;
		}

		bool onlySurroundedByGrass = GrassSpreadTick(currentWorld, pos);

		if (onlySurroundedByGrass && (!blockOnTopOfGrass)) {
			return;
		}

		Event::BlockEvent grassSpread{pos, g_blocks.GRASS, EventHandler.BlockTick};
		currentWorld->event_manager_.AddEvent(grassSpread);
	}

	bool GrassDestroyTick(Dimension* currentWorld, const BlockPos& pos) {
		//Chance it -doesn't break-
		if (TestProbability(1 - properties_.break_chance_)) {
			return false;
		}

		currentWorld->world_interactions_.SetBlock(g_blocks.DIRT, pos);

		return true;
	}

	//return true if there is no exposed dirt blocks surrounding it
	// TODO: refactor code
	bool GrassSpreadTick(Dimension* currentWorld, const BlockPos& pos) {
		bool dirtExposed = false;

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
					if (currentWorld->world_interactions_.GetBlock(newPos) != g_blocks.DIRT) {
						continue;
					}

					//Checks if there isnt any block above
					newPos.y += 1;
					if (currentWorld->world_interactions_.GetBlock(newPos) != g_blocks.AIR) {
						continue;
					}
					newPos.y -= 1;

					//Chance it spread
					if (TestProbability(properties_.spread_chance_)) {

						Event::BlockEvent blockEvent{newPos,g_blocks.GRASS, EventHandler.BlockPlace};
						currentWorld->event_manager_.AddEvent(blockEvent);

						continue;
					}

					dirtExposed = true;
				}
			}
		}

		return !dirtExposed;
	}
};
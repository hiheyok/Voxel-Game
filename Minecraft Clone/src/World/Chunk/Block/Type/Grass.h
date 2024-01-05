#pragma once
#include "../../../Event/EventSystem.h"
#include "../Block.h"
#include "../../../World.h"
#include "../../../../Utils/Math/Probability/Probability.h"
#include "../../../Event/EventHandler.h"

struct GrassProperties {
	double SpreadChance = 0.001;
	double BreakChance = 0.001;
};

struct GrassBlock : Block {

	GrassProperties Properties;

	void tick(int x, int y, int z) override {
		World* CurrentWorld = static_cast<World*>(Block::WorldPTR);

		//Checks if ticking block changes 
		if (CurrentWorld->GetBlock(x, y, z) != Blocks.GRASS) {
			return;
		}

		//If there is space above then preserve the grass block
		if (CurrentWorld->GetBlock(x, y + 1, z) == Blocks.AIR) {
			return;
		}

		//Chance it -doesn't break-
		if (TestProbability(1 - Properties.BreakChance)) {
			Event event;
			event.Type = BLOCK_EVENT;
			event.Data.BlockEvent.id = EventHandler.BlockTick;
			event.Data.BlockEvent.x = x;
			event.Data.BlockEvent.y = y;
			event.Data.BlockEvent.z = z;
			event.Data.BlockEvent.block = Blocks.GRASS;
			CurrentWorld->QueueEvent(event);
			return;
		}

		CurrentWorld->SetBlock(Blocks.DIRT, x, y, z);



	}
};
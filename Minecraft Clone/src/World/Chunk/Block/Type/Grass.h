#pragma once
#include "../../../Event/EventSystem.h"
#include "../Block.h"
#include "../../../World.h"
#include "../../../../Utils/Math/Probability/Probability.h"
#include "../../../Event/EventHandler.h"

struct GRASS_BLOCK : Block {
	void tick(int x, int y, int z) override {
		World* CurrentWorld = static_cast<World*>(Block::WorldPTR);

		//If there is space above then preserve the grass block
		if (CurrentWorld->GetBlock(x, y + 1, z) == Blocks.AIR) {
			return;
		}

		double GRASS_DESTROY_ODDS = 0.0001; //0.1% Per tick

		if (TestProbability(1 - GRASS_DESTROY_ODDS)) {
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
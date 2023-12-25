#pragma once
#include "../Block.h"
#include "../../../World.h"

struct FLUID_BLOCK : Block {
	void tick(int x, int y, int z) override {

		World* CurrentWorld = static_cast<World*>(Block::WorldPTR);

		//BlockID CurrentBlock = CurrentWorld->GetBlock(x, y, z);

		for (int side = 0; side < 6; side++) {
			int pos[3]{x, y, z};

			pos[side >> 1] += 2 * (side & 0b1) - 1;

			if (((side >> 1) == 1) && ((side & 0b1) == 0b1)) {
				continue;
			}

			BlockID block = CurrentWorld->GetBlock(pos[0], pos[1], pos[2]);

			if (block != Blocks.AIR) {
				continue;
			}

			if (CurrentWorld->CheckTickUsed(x, y, z)) {
				return;
			}

			CurrentWorld->TickUsed(x, y, z);

			Event event;
			event.Type = BLOCK_EVENT;
			event.Data.BlockEvent.id = EventHandler.BlockPlace;
			event.Data.BlockEvent.x = pos[0];
			event.Data.BlockEvent.y = pos[1];
			event.Data.BlockEvent.z = pos[2];
			event.Data.BlockEvent.block = Blocks.WATER;
			CurrentWorld->QueueEvent(event);

		}
	}
};
#pragma once
#include "../Block.h"
#include "../../../../Level/Dimension/Dimension.h"

struct FluidProperties {
	int SpreadRate = 1; // Ticks for fluid to spread
};

struct Fluid : Block {

	FluidProperties Properties;

	void tick(int x, int y, int z) override {

		Dimension* CurrentWorld = static_cast<Dimension*>(Block::DimensionPTR);

		//BlockID CurrentBlock = CurrentWorld->GetBlock(x, y, z);

		for (int side = 0; side < 6; side++) {
			int pos[3]{x, y, z};

			pos[side >> 1] += 2 * (side & 0b1) - 1;

			if (((side >> 1) == 1) && ((side & 0b1) == 0b1)) {
				continue;
			}

			BlockID block = CurrentWorld->worldInteractions.getBlock(pos[0], pos[1], pos[2]);

			if (block != Blocks.AIR) {
				continue;
			}

			if (block == Blocks.WATER) {
				continue;
			}

			if (CurrentWorld->CheckTickUsed(EventHandler.BlockPlace, x, y, z)) {
				return;
			}

			CurrentWorld->TickUsed(EventHandler.BlockPlace, x, y, z);

			Event event;
			event.Type = BLOCK_EVENT;
			event.Data.BlockEvent.id = EventHandler.BlockPlace;
			event.Data.BlockEvent.x = pos[0];
			event.Data.BlockEvent.y = pos[1];
			event.Data.BlockEvent.z = pos[2];
			event.Data.BlockEvent.block = Blocks.WATER;
			CurrentWorld->EventManager.AddEvent(event);

		}
	}
};
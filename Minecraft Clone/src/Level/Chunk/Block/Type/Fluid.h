#pragma once
#include "../../../../Level/Dimension/Dimension.h"

class Block;

struct FluidProperties {
	int SpreadRate = 1; // Ticks for fluid to spread
};


struct Fluid : Block {

	FluidProperties Properties;

	void tick(const BlockPos& pos) override {

		Dimension* CurrentWorld = static_cast<Dimension*>(Block::DimensionPTR);

		//BlockID CurrentBlock = CurrentWorld->GetBlock(x, y, z);

		for (int side = 0; side < 6; side++) {
			BlockPos newPos = pos;

			newPos[side >> 1] += (side & 0b1) * 2 - 1;

			if (((side >> 1) == 1) && ((side & 0b1) == 0b1)) {
				continue;
			}

			BlockID block = CurrentWorld->worldInteractions.getBlock(pos);

			if (block != Blocks.AIR) {
				continue;
			}

			if (block == Blocks.WATER) {
				continue;
			}

			if (CurrentWorld->CheckTickUsed(EventHandler.BlockPlace, pos)) {
				return;
			}

			CurrentWorld->TickUsed(EventHandler.BlockPlace, pos);

			Event::BlockEvent blockEvent{newPos, Blocks.WATER, EventHandler.BlockPlace};
			CurrentWorld->EventManager.AddEvent(blockEvent);

		}
	}
};
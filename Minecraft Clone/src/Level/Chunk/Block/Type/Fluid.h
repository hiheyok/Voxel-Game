#pragma once
#include "../../../../Level/Dimension/Dimension.h"

class Block;

struct FluidProperties {
	int spread_rate_ = 1; // Ticks for fluid to spread
};

struct Fluid : Block {

	FluidProperties properties_;

	void Tick(const BlockPos& pos) override {

		Dimension* currentWorld = static_cast<Dimension*>(Block::dimension_ptr_);

		//BlockID CurrentBlock = currentWorld->GetBlock(x, y, z);

		for (int side = 0; side < 6; side++) {
			BlockPos newPos = pos;

			newPos[side >> 1] += (side & 0b1) * 2 - 1;

			if (((side >> 1) == 1) && ((side & 0b1) == 0b1)) {
				continue;
			}

			BlockID block = currentWorld->world_interactions_.GetBlock(pos);

			if (block != Blocks.AIR) {
				continue;
			}

			if (block == Blocks.WATER) {
				continue;
			}

			if (currentWorld->CheckTickUsed(EventHandler.BlockPlace, pos)) {
				return;
			}

			currentWorld->TickUsed(EventHandler.BlockPlace, pos);

			Event::BlockEvent blockEvent{newPos, Blocks.WATER, EventHandler.BlockPlace};
			currentWorld->event_manager_.AddEvent(blockEvent);

		}
	}
};
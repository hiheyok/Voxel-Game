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

		for (int side = 0; side < 6; side++) {
			BlockPos newPos = pos;

			newPos.incrementSide(side, 1);

			if (side == 3) { // y_up direction
				continue;
			}

			BlockID block = currentWorld->world_interactions_.GetBlock(pos);

			if (block != g_blocks.AIR) {
				continue;
			}

			if (block == g_blocks.WATER) {
				continue;
			}

			if (currentWorld->CheckTickUsed(g_event_handler.BlockPlace, pos)) {
				return;
			}

			currentWorld->TickUsed(g_event_handler.BlockPlace, pos);

			Event::BlockEvent blockEvent{newPos, g_blocks.WATER, g_event_handler.BlockPlace};
			currentWorld->event_manager_.AddEvent(blockEvent);

		}
	}
};
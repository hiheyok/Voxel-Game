#pragma once
#include "../../../Event/EventHandler.h"
#include "../../../Entity/Entities.h"

class Block;

struct GravityBlock : Block {

	void tick(const BlockPos& pos) override {
		Dimension* CurrentWorld = static_cast<Dimension*>(Block::DimensionPTR);

		BlockPos belowPos = pos;
		belowPos.y -= 1;
		bool isBlockSupported = Blocks.getBlockType(CurrentWorld->worldInteractions.getBlock(belowPos))->Properties->isSolid;

		if (!isBlockSupported) {
			Event::BlockEvent destroyBlock{pos, Blocks.AIR, EventHandler.BlockPlace};
			Event::EntityEvent summonSand;

			summonSand.id_ = EventHandler.SummonEntity;
			summonSand.pos_ = pos;
			summonSand.entity_type_ = EntityList.SAND_GRAVITY_BLOCK;

			CurrentWorld->EventManager.AddEvent(destroyBlock);
			CurrentWorld->EventManager.AddEvent(summonSand);
		}
	}
};
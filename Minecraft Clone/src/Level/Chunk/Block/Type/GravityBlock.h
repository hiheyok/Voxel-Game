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
			Event destroyBlock;
			destroyBlock.Type = BLOCK_EVENT;
			destroyBlock.Data.BlockEvent.id = EventHandler.BlockPlace;
			destroyBlock.Data.BlockEvent.pos = pos;
			destroyBlock.Data.BlockEvent.block = Blocks.AIR;
			//destroyBlock.Data.BlockEvent.BypassLimit = 1;

			Event summonSand;
			summonSand.Type = ENTITY_EVENT;
			summonSand.Data.EntityEvent.id = EventHandler.SummonEntity;
			summonSand.Data.EntityEvent.pos = pos;
			summonSand.Data.EntityEvent.entityType = EntityList.SAND_GRAVITY_BLOCK; 

			CurrentWorld->EventManager.AddEvent(destroyBlock);
			CurrentWorld->EventManager.AddEvent(summonSand);
		}
	}
};
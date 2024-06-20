#pragma once
#include "../Block.h"
#include "../../../Event/EventHandler.h"
#include "../../../Entity/Entities.h"

struct GravityBlock : Block {

	void tick(int x, int y, int z) override {
		Dimension* CurrentWorld = static_cast<Dimension*>(Block::DimensionPTR);

		bool isBlockSupported = Blocks.getBlockType(CurrentWorld->worldInteractions.getBlock(x, y - 1, z))->Properties->isSolid;

		if (!isBlockSupported) {
			Event destroyBlock;
			destroyBlock.Type = BLOCK_EVENT;
			destroyBlock.Data.BlockEvent.id = EventHandler.BlockPlace;
			destroyBlock.Data.BlockEvent.x = x;
			destroyBlock.Data.BlockEvent.y = y;
			destroyBlock.Data.BlockEvent.z = z;
			destroyBlock.Data.BlockEvent.block = Blocks.AIR;
			//destroyBlock.Data.BlockEvent.BypassLimit = 1;

			Event summonSand;
			summonSand.Type = ENTITY_EVENT;
			summonSand.Data.EntityEvent.id = EventHandler.SummonEntity;
			summonSand.Data.EntityEvent.x = x;
			summonSand.Data.EntityEvent.y = y;
			summonSand.Data.EntityEvent.z = z;
			summonSand.Data.EntityEvent.entityType = EntityList.SAND_GRAVITY_BLOCK;

			CurrentWorld->EventManager.AddEvent(destroyBlock);
			CurrentWorld->EventManager.AddEvent(summonSand);
		}
	}
};
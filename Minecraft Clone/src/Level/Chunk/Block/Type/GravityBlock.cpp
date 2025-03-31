#include "GravityBlock.h"
#include "../../../Dimension/Dimension.h"
#include "../../../Event/EventHandler.h"
#include "../../../Entity/Entities.h"

void GravityBlock::Tick(const BlockPos& pos, Dimension* currentWorld) {
    BlockPos belowPos = pos;
    belowPos.y -= 1;
    bool isBlockSupported = g_blocks.GetBlockType(currentWorld->world_interactions_.GetBlock(belowPos))->properties_->is_solid_;

    if (!isBlockSupported) {
        Event::BlockEvent destroyBlock{ pos, g_blocks.AIR, g_event_handler.BlockPlace };
        Event::EntityEvent summonSand;

        summonSand.id_ = g_event_handler.SummonEntity;
        summonSand.pos_ = pos;
        summonSand.entity_type_ = g_entity_list.SAND_GRAVITY_BLOCK;

        currentWorld->event_manager_.AddEvent(destroyBlock);
        currentWorld->event_manager_.AddEvent(summonSand);
    }
}
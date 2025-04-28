#include "Level/Block/Type/Grass.h"

#include "Level/Dimension/Dimension.h"
#include "Level/Event/EventHandler.h"
#include "Level/World/WorldUpdater.h"
#include "Utils/Math/Probability/Probability.h"

GrassBlock::GrassBlock(double spread_chance, double break_chance) {
    properties_->is_solid_ = true;
    properties_->transparency_ = false;
    properties_->is_fluid_ = false;
    properties_->light_pass_ = false;

    grass_properties_.spread_chance_ = spread_chance;
    grass_properties_.break_chance_ = break_chance;
}

void GrassBlock::Tick(const BlockPos& pos, Dimension* currentWorld) {
    // Checks if ticking block changes
    if (currentWorld->world_->GetBlock(pos) != g_blocks.GRASS) {
        return;
    }

    BlockPos newPos = pos;
    newPos.y += 1;

    bool blockOnTopOfGrass =
        (currentWorld->world_->GetBlock(newPos) != g_blocks.AIR);

    bool isGrassDestroyed = false;

    if (blockOnTopOfGrass) {
        isGrassDestroyed = GrassDestroyTick(currentWorld, pos);
    }

    // If grass destroyed tick ends
    if (isGrassDestroyed) {
        return;
    }

    bool onlySurroundedByGrass = GrassSpreadTick(currentWorld, pos);

    if (onlySurroundedByGrass && (!blockOnTopOfGrass)) {
        return;
    }

    BlockEvent grassSpread{pos, g_blocks.GRASS, g_event_handler.BlockTick};
    currentWorld->event_manager_.AddEvent(grassSpread);
}

bool GrassBlock::GrassDestroyTick(Dimension* currentWorld,
                                  const BlockPos& pos) {
    // Chance it -doesn't break-
    if (TestProbability(1 - grass_properties_.break_chance_)) {
        return false;
    }

    currentWorld->world_updater_->SetBlock(g_blocks.DIRT, pos);

    return true;
}

bool GrassBlock::GrassSpreadTick(Dimension* currentWorld, const BlockPos& pos) {
    bool dirtExposed = false;

    for (int x1 = -1; x1 <= 1; x1++) {
        for (int z1 = -1; z1 <= 1; z1++) {
            if (x1 == 0 && z1 == 0) {
                continue;
            }

            for (int y1 = -1; y1 <= 1; y1++) {
                BlockPos newPos = pos;
                newPos.x += x1;
                newPos.y += y1;
                newPos.z += z1;

                // Checks if block is dirt
                if (currentWorld->world_->GetBlock(newPos) != g_blocks.DIRT) {
                    continue;
                }

                // Checks if there isnt any block above
                newPos.y += 1;
                if (currentWorld->world_->GetBlock(newPos) != g_blocks.AIR) {
                    continue;
                }
                newPos.y -= 1;

                // Chance it spread
                if (TestProbability(grass_properties_.spread_chance_)) {
                    BlockEvent blockEvent{newPos, g_blocks.GRASS,
                                          g_event_handler.BlockPlace};
                    currentWorld->event_manager_.AddEvent(blockEvent);

                    continue;
                }

                dirtExposed = true;
            }
        }
    }

    return !dirtExposed;
}
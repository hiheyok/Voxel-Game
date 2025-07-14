// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/Grass.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Event/EventHandler.h"
#include "Level/World/WorldUpdater.h"
#include "Utils/Math/Probability/Probability.h"

GrassBlock::GrassBlock(GameContext& game_context, double spread_chance,
                       double break_chance)
    : Block{game_context} {
  properties_->is_solid_ = true;
  properties_->transparency_ = false;
  properties_->is_fluid_ = false;
  properties_->light_pass_ = false;


  grass_properties_.spread_chance_ = spread_chance;
  grass_properties_.break_chance_ = break_chance;
}

void GrassBlock::Tick(BlockPos pos, Dimension* currentWorld) {
  // Checks if ticking block changes
  if (currentWorld->world_->GetBlock(pos) != game_context_.blocks_->GRASS) {
    return;
  }

  BlockPos newPos = pos;
  newPos.y += 1;

  bool blockOnTopOfGrass =
      (currentWorld->world_->GetBlock(newPos) != game_context_.blocks_->AIR);

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

  BlockEvent grassSpread{pos, game_context_.blocks_->GRASS,
                         game_context_.event_handler_->BlockTick};
  currentWorld->event_manager_.AddEvent(grassSpread);
}

bool GrassBlock::GrassDestroyTick(Dimension* currentWorld, BlockPos pos) {
  // Chance it -doesn't break-
  if (TestProbability(1 - grass_properties_.break_chance_)) {
    return false;
  }

  currentWorld->world_updater_->SetBlock(game_context_.blocks_->DIRT, pos);

  return true;
}

bool GrassBlock::GrassSpreadTick(Dimension* currentWorld, BlockPos pos) {
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
        if (currentWorld->world_->GetBlock(newPos) !=
            game_context_.blocks_->DIRT) {
          continue;
        }

        // Checks if there isnt any block above
        newPos.y += 1;
        if (currentWorld->world_->GetBlock(newPos) !=
            game_context_.blocks_->AIR) {
          continue;
        }
        newPos.y -= 1;

        // Chance it spread
        if (TestProbability(grass_properties_.spread_chance_)) {
          BlockEvent blockEvent{newPos, game_context_.blocks_->GRASS,
                                game_context_.event_handler_->BlockPlace};
          currentWorld->event_manager_.AddEvent(blockEvent);

          continue;
        }

        dirtExposed = true;
      }
    }
  }

  return !dirtExposed;
}

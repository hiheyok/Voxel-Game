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

void GrassBlock::Tick(BlockPos pos, Dimension* curr_world) {
  WorldInterface* world = curr_world->world_;

  // Checks if ticking block changes
  if (world->GetBlock(pos) != game_context_.blocks_->GRASS) {
    return;
  }

  BlockPos top_pos = pos;
  top_pos.y += 1;

  bool top_is_not_air = world->GetBlock(top_pos) != game_context_.blocks_->AIR;

  bool destroy_grass = false;

  if (top_is_not_air) {
    destroy_grass = GrassDestroyTick(curr_world, pos);
  }

  // If grass destroyed tick ends
  if (destroy_grass) {
    return;
  }

  bool onlySurroundedByGrass = GrassSpreadTick(curr_world, pos);

  if (onlySurroundedByGrass && (!top_is_not_air)) {
    return;
  }

  BlockEvent grass_spread{pos, game_context_.blocks_->GRASS,
                          game_context_.event_handler_->BlockTick};
  curr_world->event_manager_.AddEvent(grass_spread);
}

bool GrassBlock::GrassDestroyTick(Dimension* curr_world, BlockPos pos) {
  // Chance it -doesn't break-
  if (TestProbability(1 - grass_properties_.break_chance_)) {
    return false;
  }

  curr_world->world_updater_->SetBlock(game_context_.blocks_->DIRT, pos);

  return true;
}

bool GrassBlock::GrassSpreadTick(Dimension* curr_world, BlockPos pos) {
  bool dirtExposed = false;

  for (auto [x1, z1, y1] : Product<3>(-1, 2)) {
    if (x1 == 0 && z1 == 0) {
      continue;
    }
    BlockPos newPos = pos;
    newPos.x += x1;
    newPos.y += y1;
    newPos.z += z1;

    // Checks if block is dirt
    if (curr_world->world_->GetBlock(newPos) != game_context_.blocks_->DIRT) {
      continue;
    }

    // Checks if there isnt any block above
    newPos.y += 1;
    if (curr_world->world_->GetBlock(newPos) != game_context_.blocks_->AIR) {
      continue;
    }
    newPos.y -= 1;

    // Chance it spread
    if (TestProbability(grass_properties_.spread_chance_)) {
      BlockEvent blockEvent{newPos, game_context_.blocks_->GRASS,
                            game_context_.event_handler_->BlockPlace};
      curr_world->event_manager_.AddEvent(blockEvent);

      continue;
    }

    dirtExposed = true;
  }

  return !dirtExposed;
}

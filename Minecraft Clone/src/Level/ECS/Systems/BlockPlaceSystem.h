#pragma once

#include "Level/ECS/Components/BlockPlaceComponent.h"
#include "Level/ECS/EntitySystem.h"

/**
 * Store what block type to place
 * Track how long entity has been falling
 * Monitor collusion system for ground collusion
 * Validate placement conditions
 * Delete entity and place block in world
 * Convert to item if placement fails (sand lands on torch -> sand item)
 */

class BlockPlaceSystem : public EntitySystem<BlockPlaceComponent> {
 public:
  BlockPlaceSystem(GameContext& context, WorldInterface& world, EntitySystems& entity_systems);
  void Tick() override;
};


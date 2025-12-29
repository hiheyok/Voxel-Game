#pragma once

#include "Level/ECS/Components/BlockPlaceComponent.h"
#include "Level/ECS/EntitySystem.h"

class WorldUpdater;

/**
 * Store what block type to place
 * Track how long entity has been falling
 * Monitor collision system for ground collision
 * Validate placement conditions
 * Delete entity and place block in world
 * Convert to item if placement fails (sand lands on torch -> sand item)
 */

class BlockPlaceSystem : public EntitySystem<BlockPlaceComponent> {
 public:
  BlockPlaceSystem(GameContext& context, WorldInterface& world, ServerEntitySystems& entity_systems);
  void Tick() override;
  
  void SetWorldUpdater(WorldUpdater* updater);

 private:
  WorldUpdater* world_updater_ = nullptr;
};

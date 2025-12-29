#pragma once

#include "Level/ECS/Components/GravityComponent.h"
#include "Level/ECS/EntitySystem.h"

/**
 * Store gravity scale for each entity
 * Store terminal velocity
 * Check if entity is grounded
 * Apply downward acceleration to non grounded entities to transform system
 * Water buoyancy, levitation
 */

class GravitySystem : public EntitySystem<GravityComponent> {
 public:
  GravitySystem(GameContext& context, WorldInterface& world, ServerEntitySystems& entity_systems);
  void Tick() override;
};


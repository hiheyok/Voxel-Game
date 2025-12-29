#pragma once

#include "Level/ECS/Components/CollisionComponent.h"
#include "Level/ECS/EntitySystem.h"

/**
 * Handle world and entity to entity collisions
 * Penetration depth and collision normals
 * Collision flags (grounded, touching_wall, in_water, etc)
 * Resolve collision
 * Track what entities are standing on
 * Handle trigger zones (pressure plate, area effects, etc)
 */

class CollisionSystem : public EntitySystem<CollisionComponent> {
 public:
  CollisionSystem(GameContext& context, WorldInterface& world, ServerEntitySystems& entity_systems);
  void Tick() override;

  bool IsGround(EntityUUID uuid) const noexcept;
  bool GetCollisionStatus(EntityUUID uuid, int side) const noexcept;
  float TimeTillCollision(EntityUUID uuid) const noexcept;
};


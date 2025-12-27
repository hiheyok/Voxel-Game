#pragma once

#include "Level/ECS/Components/TransformComponent.h"
#include "Level/ECS/EntitySystem.h"

/**
 * Store position, velocity, acceleration for each entity
 * Store rotations
 * Apply physics integration (position += velocity * dt)
 */

class TransformSystem : public EntitySystem<TransformComponent> {
 public:
  TransformSystem(GameContext& context, WorldInterface& world, EntitySystems& entity_systems);
  
  void Tick() override;

  void SetPosition(EntityUUID uuid, glm::vec3 position) noexcept;
  void SetVelocity(EntityUUID uuid, glm::vec3 velocity) noexcept;
  void SetAcceleration(EntityUUID uuid, glm::vec3 acceleration) noexcept;

  void OffsetPosition(EntityUUID uuid, glm::vec3 position) noexcept;
  void OffsetVelocity(EntityUUID uuid, glm::vec3 velocity) noexcept;
  void OffsetAcceleration(EntityUUID uuid, glm::vec3 acceleration) noexcept;

 protected:
  void OnCommit() override;
};

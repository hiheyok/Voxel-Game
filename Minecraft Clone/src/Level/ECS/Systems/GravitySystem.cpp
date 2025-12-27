#include "Level/ECS/Systems/GravitySystem.h"

#include "Core/GameContext/GameContext.h"
#include "Level/ECS/ComponentMap.h"
#include "Level/ECS/EntitySystems.h"
#include "Level/ECS/Systems/TransformSystem.h"
#include "Level/World/WorldInterface.h"

GravitySystem::GravitySystem(GameContext& context, WorldInterface& world, EntitySystems& entity_systems)
    : EntitySystem{context, world, entity_systems} {}

void GravitySystem::Tick() {
  const float world_gravity = -0.05f; // TMP

  for (const auto& [uuid, gravity_comp] : components_active_) {
    if (!gravity_comp.is_affected_by_gravity_) {
      continue;
    }

    // Get transform component to apply acceleration
    auto& transform_system = entity_systems_.GetTransformSystem();
    if (!transform_system.HasComponent(uuid)) {
      continue;
    }

    const auto& transform_comp = transform_system.GetComponent(uuid);

    // Apply gravity acceleration (negative Y direction)
    float gravity_accel = world_gravity * gravity_comp.gravity_multiplier_;
    transform_system.OffsetAcceleration(uuid, glm::vec3(0.0f, gravity_accel, 0.0f));

    // Apply terminal velocity limit
    if (transform_comp.velocity_.y < -gravity_comp.terminal_velocity_) {
      transform_system.SetVelocity(uuid, glm::vec3(transform_comp.velocity_.x,
                                                     -gravity_comp.terminal_velocity_,
                                                     transform_comp.velocity_.z));
    }
  }
}

#include "Level/ECS/Systems/GravitySystem.h"

#include "Core/GameContext/GameContext.h"
#include "Level/ECS/ComponentMap.h"
#include "Level/ECS/ServerEntitySystems.h"
#include "Level/ECS/Systems/TickStateSystem.h"
#include "Level/ECS/Systems/TransformSystem.h"
#include "Level/World/WorldInterface.h"

GravitySystem::GravitySystem(GameContext& context, WorldInterface& world,
                             ServerEntitySystems& entity_systems)
    : EntitySystem{context, world, entity_systems} {}

void GravitySystem::Tick() {
  const float world_gravity = -0.05f;  // TMP
  auto& tick_state_system = entity_systems_.GetConcreteTickStateSystem();

  for (const auto& [uuid, gravity_comp] : components_active_) {
    if (tick_state_system.HasComponent(uuid) &&
        !tick_state_system.IsActive(uuid)) {
      continue;
    }
    if (!gravity_comp.is_affected_by_gravity_) {
      continue;
    }

    // Get transform component to apply acceleration
    auto& transform_system = entity_systems_.GetConcreteTransformSystem();
    if (!transform_system.HasComponent(uuid)) {
      continue;
    }

    const auto& transform_comp = transform_system.GetComponent(uuid);

    // Apply gravity acceleration (negative Y direction)
    float gravity_accel = world_gravity * gravity_comp.gravity_multiplier_;
    transform_system.OffsetAcceleration(uuid,
                                        glm::vec3(0.0f, gravity_accel, 0.0f));

    // Apply terminal velocity limit
    if (transform_comp.velocity_.y < -gravity_comp.terminal_velocity_) {
      transform_system.SetVelocity(uuid,
                                   glm::vec3(transform_comp.velocity_.x,
                                             -gravity_comp.terminal_velocity_,
                                             transform_comp.velocity_.z));
    }
  }
}

#include "Level/ECS/Systems/TransformSystem.h"

#include <glm/vec3.hpp>

#include "Core/Typenames.h"
#include "Level/ECS/ComponentMap.h"
#include "Level/ECS/Components/TransformComponent.h"
#include "Level/ECS/EntitySystem.h"
#include "Level/ECS/ServerEntitySystems.h"
#include "Level/ECS/Systems/TickStateSystem.h"
#include "Level/World/WorldInterface.h"

using glm::vec3;

TransformSystem::TransformSystem(GameContext& context, WorldInterface& world,
                                 ServerEntitySystems& entity_systems)
    : EntitySystem{context, world, entity_systems} {}

void TransformSystem::Tick() {
  auto& tick_state_system = entity_systems_.GetConcreteTickStateSystem();

  for (const auto& [uuid, component] : components_active_) {
    if (tick_state_system.HasComponent(uuid) &&
        !tick_state_system.IsActive(uuid)) {
      continue;
    }
    OffsetPosition(uuid, component.velocity_);
    OffsetVelocity(uuid, component.acceleration_);
  }
}

// ITransformSystem interface implementations
const ComponentMap<TransformComponent>& TransformSystem::GetComponentMap()
    const {
  return EntitySystem<TransformComponent>::GetComponentMap();
}

void TransformSystem::ReplaceComponent(EntityUUID uuid,
                                       const TransformComponent& component) {
  EntitySystem<TransformComponent>::ReplaceComponent(uuid, component);
}

void TransformSystem::SetPosition(EntityUUID uuid, vec3 position) noexcept {
  components_updated_[uuid].position_ = position;
}
void TransformSystem::SetVelocity(EntityUUID uuid, vec3 velocity) noexcept {
  components_updated_[uuid].velocity_ = velocity;
}
void TransformSystem::SetAcceleration(EntityUUID uuid,
                                      vec3 acceleration) noexcept {
  components_updated_[uuid].acceleration_ = acceleration;
}

void TransformSystem::OffsetPosition(EntityUUID uuid, vec3 position) noexcept {
  components_updated_[uuid].position_ += position;
}
void TransformSystem::OffsetVelocity(EntityUUID uuid, vec3 velocity) noexcept {
  components_updated_[uuid].velocity_ += velocity;
}
void TransformSystem::OffsetAcceleration(EntityUUID uuid,
                                         vec3 acceleration) noexcept {
  components_updated_[uuid].acceleration_ += acceleration;
}

void TransformSystem::OnCommit() {
  for (auto& [_, component] : components_updated_) {
    component.acceleration_ = vec3(0.0f);
  }
}

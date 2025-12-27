#include "Level/ECS/Systems/TransformSystem.h"

#include "Level/ECS/ComponentMap.h"
#include "Level/ECS/EntitySystems.h"
#include "Level/World/WorldInterface.h"

TransformSystem::TransformSystem(GameContext& context, WorldInterface& world, EntitySystems& entity_systems)
    : EntitySystem{context, world, entity_systems} {}


void TransformSystem::Tick() {
  const float mspt = 1.0 / world_.parameters.world_tps_;

  for (const auto& [uuid, component] : components_active_) {
    OffsetPosition(uuid, component.velocity_ * mspt);
    OffsetVelocity(uuid, component.acceleration_ * mspt);
  }
}

void TransformSystem::SetPosition(EntityUUID uuid,
                                  glm::vec3 position) noexcept {
  components_updated_.GetComponent(uuid).position_ = position;
}
void TransformSystem::SetVelocity(EntityUUID uuid,
                                  glm::vec3 velocity) noexcept {
  components_updated_.GetComponent(uuid).velocity_ = velocity;
}
void TransformSystem::SetAcceleration(EntityUUID uuid,
                                      glm::vec3 acceleration) noexcept {
  components_updated_.GetComponent(uuid).acceleration_ = acceleration;
}

void TransformSystem::OffsetPosition(EntityUUID uuid,
                                     glm::vec3 position) noexcept {
  components_updated_.GetComponent(uuid).position_ += position;
}
void TransformSystem::OffsetVelocity(EntityUUID uuid,
                                     glm::vec3 velocity) noexcept {
  components_updated_.GetComponent(uuid).velocity_ += velocity;
}
void TransformSystem::OffsetAcceleration(EntityUUID uuid,
                                         glm::vec3 acceleration) noexcept {
  components_updated_.GetComponent(uuid).acceleration_ += acceleration;
}

void TransformSystem::OnCommit() {
  for (auto& [_, component] : components_updated_) {
    component.acceleration_ = glm::vec3(0.0f);
  }
}

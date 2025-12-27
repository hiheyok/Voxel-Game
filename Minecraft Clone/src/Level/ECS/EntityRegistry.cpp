#include "Level/ECS/EntityRegistry.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/ECS/Components/BlockComponent.h"
#include "Level/ECS/Components/BlockPlaceComponent.h"
#include "Level/ECS/Components/CollisionComponent.h"
#include "Level/ECS/Components/GravityComponent.h"
#include "Level/ECS/EntitySystems.h"
#include "Level/ECS/Systems/BlockPlaceSystem.h"
#include "Level/ECS/Systems/BlockSystem.h"
#include "Level/ECS/Systems/CollisionSystem.h"
#include "Level/ECS/Systems/GravitySystem.h"
#include "Level/ECS/Systems/TransformSystem.h"
#include "Utils/Assert.h"

EntityRegistry::EntityRegistry(GameContext& context,
                               EntitySystems& entity_systems)
    : context_{context}, entity_systems_{entity_systems} {}

EntityType EntityRegistry::GetEntityType(EntityUUID uuid) const noexcept {
  GAME_ASSERT(IsValidUUID(uuid), "Invalid UUID");
  return uuid_id_mapping_[uuid];
}

bool EntityRegistry::IsValidUUID(EntityUUID uuid) const noexcept {
  return uuid < uuid_id_mapping_.size() &&
         uuid_id_mapping_[uuid] != EntityType::kInvalidEntity;
}

EntityUUID EntityRegistry::CreateSand(glm::vec3 position) {
  EntityUUID entity = GetNewUUID(EntityType::kFallingBlock);
  
  // Add Transform component (position, velocity, acceleration)
  TransformSystem& transform_system = entity_systems_.GetTransformSystem();
  TransformComponent& transform_comp = transform_system.AddComponent(entity);
  transform_comp.position_ = position;
  transform_comp.velocity_ = glm::vec3(0.0f);
  transform_comp.acceleration_ = glm::vec3(0.0f);

  // Add Gravity component (enable gravity)
  auto& gravity_system = entity_systems_.GetGravitySystem();
  GravityComponent& gravity_comp = gravity_system.AddComponent(entity);
  gravity_comp.is_affected_by_gravity_ = true;
  gravity_comp.gravity_multiplier_ = 1.0f;
  gravity_comp.terminal_velocity_ = 50.0f;

  // Add Collision component (for grounded detection)
  auto& collision_system = entity_systems_.GetCollisionSystem();
  CollisionComponent& collision_comp = collision_system.AddComponent(entity);
  collision_comp.grounded = false;

  // Add Block component (store sand block ID)
  auto& block_system = entity_systems_.GetBlockSystem();
  BlockComponent& block_comp = block_system.AddComponent(entity);
  block_comp.block_ = context_.blocks_->SAND;

  // Add BlockPlace component (enable conversion to block when grounded)
  auto& block_place_system = entity_systems_.GetBlockPlaceSystem();
  BlockPlaceComponent& place_comp = block_place_system.AddComponent(entity);
  place_comp.block_id_ = context_.blocks_->SAND;
  place_comp.should_convert_ = true;
  place_comp.break_on_impact_ = false;

  return entity;
}


EntityUUID EntityRegistry::GetNewUUID(EntityType entity_type) {
  EntityUUID uuid = -1;

  // Check the stack if there are any remaining uuids

  if (!leftover_uuids_.empty()) {
    uuid = leftover_uuids_.top();
    leftover_uuids_.pop();
  } else {
    uuid = uuid_id_mapping_.size();
    uuid_id_mapping_.emplace_back(EntityType::kInvalidEntity);
  }

  uuid_id_mapping_[uuid] = entity_type;
  return uuid;
}

void EntityRegistry::RemoveEntity(EntityUUID uuid) {
  // Remove components from all systems
  auto& transform_system = entity_systems_.GetTransformSystem();
  auto& gravity_system = entity_systems_.GetGravitySystem();
  auto& collision_system = entity_systems_.GetCollisionSystem();
  auto& block_system = entity_systems_.GetBlockSystem();
  auto& block_place_system = entity_systems_.GetBlockPlaceSystem();

  if (transform_system.HasComponent(uuid)) {
    transform_system.RemoveComponent(uuid);
  }
  if (gravity_system.HasComponent(uuid)) {
    gravity_system.RemoveComponent(uuid);
  }
  if (collision_system.HasComponent(uuid)) {
    collision_system.RemoveComponent(uuid);
  }
  if (block_system.HasComponent(uuid)) {
    block_system.RemoveComponent(uuid);
  }
  if (block_place_system.HasComponent(uuid)) {
    block_place_system.RemoveComponent(uuid);
  }

  // Remove the UUID mapping
  RemoveEntityUUID(uuid);
}

void EntityRegistry::RemoveEntityUUID(EntityUUID uuid) {
  GAME_ASSERT(uuid_id_mapping_[uuid] != EntityType::kInvalidEntity,
              "Attempted to remove already removed entity uuid");

  uuid_id_mapping_[uuid] = EntityType::kInvalidEntity;
  leftover_uuids_.push(uuid);
}

void EntityRegistry::CreateEntityWithUUID(EntityUUID uuid, EntityType type) {
  // Ensure mapping is large enough
  while (uuid_id_mapping_.size() <= uuid) {
    uuid_id_mapping_.emplace_back(EntityType::kInvalidEntity);
  }
  
  uuid_id_mapping_[uuid] = type;
  
  // Add transform component for this entity
  TransformSystem& transform_system = entity_systems_.GetTransformSystem();
  transform_system.AddComponent(uuid);
}
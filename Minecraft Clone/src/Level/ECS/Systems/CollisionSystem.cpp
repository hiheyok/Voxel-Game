#include "Level/ECS/Systems/CollisionSystem.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/ECS/ComponentMap.h"
#include "Level/ECS/ServerEntitySystems.h"
#include "Level/ECS/Systems/TickStateSystem.h"
#include "Level/ECS/Systems/TransformSystem.h"
#include "Level/World/WorldInterface.h"

CollisionSystem::CollisionSystem(GameContext& context, WorldInterface& world,
                                 ServerEntitySystems& entity_systems)
    : EntitySystem{context, world, entity_systems} {}

void CollisionSystem::Tick() {
  auto& transform_system = entity_systems_.GetConcreteTransformSystem();
  auto& tick_state_system = entity_systems_.GetConcreteTickStateSystem();

  for (auto& [uuid, collision_comp] : components_updated_) {
    if (tick_state_system.HasComponent(uuid) &&
        !tick_state_system.IsActive(uuid)) {
      continue;
    }
    if (!transform_system.HasComponent(uuid)) {
      continue;
    }

    const auto& transform_comp = transform_system.GetComponent(uuid);
    glm::vec3 pos = transform_comp.position_;

    // Check the block directly below the entity
    // Entity feet are at pos.y, so check the block below that position
    int block_below_y = static_cast<int>(std::floor(pos.y)) - 1;
    BlockPos block_below(static_cast<int>(std::floor(pos.x)), block_below_y,
                         static_cast<int>(std::floor(pos.z)));

    BlockID block = world_.GetBlock(block_below);
    bool is_solid =
        context_.blocks_->GetBlockType(block)->properties_->is_solid_;

    // Check if entity is close to landing on top of the block below
    // Top of the block below is at (block_below_y + 1)
    float top_of_block_below = static_cast<float>(block_below_y + 1);
    float distance_above_block = pos.y - top_of_block_below;

    // Entity is grounded if it's within 0.15 units above the solid block
    bool is_grounded = is_solid && (distance_above_block >= 0.0f &&
                                    distance_above_block <= 0.15f);

    // If entity just became grounded, snap it to the block surface and stop
    // downward velocity
    if (is_grounded && !collision_comp.grounded) {
      transform_system.SetPosition(uuid,
                                   glm::vec3(pos.x, top_of_block_below, pos.z));
      transform_system.SetVelocity(uuid, glm::vec3(0.0f, 0.0f, 0.0f));
      transform_system.SetAcceleration(uuid, glm::vec3(0.0f, 0.0f, 0.0f));
    }

    collision_comp.grounded = is_grounded;
    collision_comp.standing_block = block;
  }
}

bool CollisionSystem::IsGround(EntityUUID uuid) const noexcept {
  GAME_ASSERT(HasComponent(uuid), "Entity doesn't have CollisionComponent");
  return GetComponent(uuid).grounded;
}

bool CollisionSystem::GetCollisionStatus(EntityUUID uuid,
                                         int side) const noexcept {
  // Not implemented for falling sand
  return false;
}

float CollisionSystem::TimeTillCollision(EntityUUID uuid) const noexcept {
  // Not implemented for falling sand
  return 0.0f;
}

#include "Level/ECS/Systems/BlockPlaceSystem.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Dimension/Dimension.h"
#include "Level/ECS/ComponentMap.h"
#include "Level/ECS/ECSManager.h"
#include "Level/ECS/EntityRegistry.h"
#include "Level/ECS/EntitySystems.h"
#include "Level/ECS/Systems/BlockSystem.h"
#include "Level/ECS/Systems/CollisionSystem.h"
#include "Level/ECS/Systems/TransformSystem.h"
#include "Level/World/WorldInterface.h"

BlockPlaceSystem::BlockPlaceSystem(GameContext& context, WorldInterface& world, EntitySystems& entity_systems)
    : EntitySystem{context, world, entity_systems} {}

void BlockPlaceSystem::Tick() {
  auto& transform_system = entity_systems_.GetTransformSystem();
  auto& collision_system = entity_systems_.GetCollisionSystem();
  auto& block_system = entity_systems_.GetBlockSystem();
  auto& registry = world_.GetECSManager().GetRegistry();

  std::vector<EntityUUID> entities_to_remove;

  for (const auto& [uuid, place_comp] : components_active_) {
    if (!place_comp.should_convert_) {
      continue;
    }

    // Check if entity is grounded
    if (!collision_system.IsGround(uuid)) {
      continue;
    }

    // Get entity position to place block
    if (!transform_system.HasComponent(uuid)) {
      continue;
    }

    const auto& transform_comp = transform_system.GetComponent(uuid);
    glm::vec3 pos = transform_comp.position_;

    // Get block ID to place
    BlockID block_id = context_.blocks_->AIR;
    if (block_system.HasComponent(uuid)) {
      block_id = block_system.GetComponent(uuid).block_;
    }

    // Place block at the block position the entity is in
    // Use floor to get the exact block coordinates
    BlockPos block_pos(static_cast<int>(std::floor(pos.x)),
                       static_cast<int>(std::floor(pos.y)),
                       static_cast<int>(std::floor(pos.z)));

    // Place block in world
    world_.GetChunkMap()->SetBlock(block_id, block_pos);

    // Mark entity for removal
    entities_to_remove.push_back(uuid);
  }

  // Remove entities that placed blocks
  for (EntityUUID uuid : entities_to_remove) {
    registry.RemoveEntity(uuid);
  }
}

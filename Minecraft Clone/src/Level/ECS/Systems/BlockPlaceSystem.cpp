#include "Level/ECS/Systems/BlockPlaceSystem.h"

#include <cmath>
#include <glm/vec3.hpp>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Block/Blocks.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Dimension/Dimension.h"
#include "Level/ECS/EntityRegistry.h"
#include "Level/ECS/EntitySystem.h"
#include "Level/ECS/ServerECSManager.h"
#include "Level/ECS/ServerEntitySystems.h"
#include "Level/ECS/Systems/BlockSystem.h"
#include "Level/ECS/Systems/CollisionSystem.h"
#include "Level/ECS/Systems/TickStateSystem.h"
#include "Level/ECS/Systems/TransformSystem.h"
#include "Level/World/WorldInterface.h"
#include "Level/World/WorldUpdater.h"

using std::floor;

BlockPlaceSystem::BlockPlaceSystem(GameContext& context, WorldInterface& world,
                                   ServerEntitySystems& entity_systems)
    : EntitySystem{context, world, entity_systems} {}

void BlockPlaceSystem::SetWorldUpdater(WorldUpdater* updater) {
  world_updater_ = updater;
}

void BlockPlaceSystem::Tick() {
  // Skip if no world updater set
  if (!world_updater_) return;

  auto& transform_system = entity_systems_.GetConcreteTransformSystem();
  auto& collision_system = entity_systems_.GetCollisionSystem();
  auto& block_system = entity_systems_.GetBlockSystem();
  auto& tick_state_system = entity_systems_.GetConcreteTickStateSystem();

  // Cast to ServerECSManager to access GetRegistry()
  auto& server_ecs = static_cast<ServerECSManager&>(world_.GetECSManager());
  auto& registry = server_ecs.GetRegistry();

  std::vector<EntityUUID> entities_to_remove;

  for (const auto& [uuid, place_comp] : components_active_) {
    continue;
    if (tick_state_system.HasComponent(uuid) &&
        !tick_state_system.IsActive(uuid)) {
      continue;
    }

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
    BlockPos block_pos(floor(pos.x), floor(pos.y), floor(pos.z));

    // Place block in world through WorldUpdater (handles tracking)
    world_updater_->SetBlock(block_id, block_pos);

    // Mark entity for removal
    entities_to_remove.push_back(uuid);
  }

  // Remove entities that placed blocks
  for (EntityUUID uuid : entities_to_remove) {
    registry.RemoveEntity(uuid);
  }
}

#include "Level/ECS/Systems/TickStateSystem.h"

#include <cmath>
#include <glm/vec3.hpp>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/ECS/EntitySystem.h"
#include "Level/ECS/ServerEntitySystems.h"
#include "Level/ECS/Systems/TransformSystem.h"
#include "Level/World/WorldInterface.h"

using std::floor;

TickStateSystem::TickStateSystem(GameContext& context, WorldInterface& world,
                                 ServerEntitySystems& entity_systems)
    : EntitySystem{context, world, entity_systems} {}

void TickStateSystem::Tick() {
  auto& transform_system = entity_systems_.GetConcreteTransformSystem();

  for (auto& [uuid, tick_state] : components_active_) {
    if (!transform_system.HasComponent(uuid)) {
      continue;
    }

    glm::vec3 pos = transform_system.GetComponent(uuid).position_;

    // Checks if this position has a valid chunk
    BlockPos block_pos(floor(pos.x), floor(pos.y), floor(pos.z));
    if (world_.CheckChunk(block_pos.ToChunkPos())) {
      SetActive(uuid);
    } else {
      SetUnactive(uuid);
    }
  }
}

void TickStateSystem::SetActive(EntityUUID uuid) noexcept {
  components_updated_[uuid].state_ = TickStateComponent::State::kActive;
}

void TickStateSystem::SetUnactive(EntityUUID uuid) noexcept {
  components_updated_[uuid].state_ = TickStateComponent::State::kUnactive;
}

// ITickStateSystem interface implementations
bool TickStateSystem::HasComponent(EntityUUID uuid) const {
  return EntitySystem<TickStateComponent>::HasComponent(uuid);
}

bool TickStateSystem::IsActive(EntityUUID uuid) const noexcept {
  return GetComponent(uuid).state_ == TickStateComponent::State::kActive;
}

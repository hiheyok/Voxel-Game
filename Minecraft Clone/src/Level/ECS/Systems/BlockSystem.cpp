#include "Level/ECS/Systems/BlockSystem.h"

#include "Core/GameContext/GameContext.h"
#include "Level/ECS/EntitySystems.h"
#include "Level/World/WorldInterface.h"

BlockSystem::BlockSystem(GameContext& context, WorldInterface& world, EntitySystems& entity_systems)
    : EntitySystem{context, world, entity_systems} {}

void BlockSystem::Tick() {
  // No per-tick logic needed for block components
}

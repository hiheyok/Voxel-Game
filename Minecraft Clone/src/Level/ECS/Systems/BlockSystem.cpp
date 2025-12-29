#include "Level/ECS/Systems/BlockSystem.h"

#include "Core/GameContext/GameContext.h"
#include "Level/ECS/ServerEntitySystems.h"
#include "Level/ECS/Systems/TickStateSystem.h"
#include "Level/World/WorldInterface.h"

BlockSystem::BlockSystem(GameContext& context, WorldInterface& world,
                         ServerEntitySystems& entity_systems)
    : EntitySystem{context, world, entity_systems} {}

void BlockSystem::Tick() {
  auto& tick_state_system = entity_systems_.GetConcreteTickStateSystem();
  // No per-tick logic needed for block components
}

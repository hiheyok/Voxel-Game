#include "Level/ECS/ECSManager.h"

#include "Level/ECS/EntityRegistry.h"
#include "Level/ECS/ServerEntitySystems.h"
#include "Level/ECS/Systems/BlockPlaceSystem.h"
#include "Level/ECS/Systems/BlockSystem.h"
#include "Level/ECS/Systems/CollisionSystem.h"
#include "Level/ECS/Systems/GravitySystem.h"
#include "Level/ECS/Systems/TickStateSystem.h"
#include "Level/ECS/Systems/TransformSystem.h"

using std::make_unique;

ECSManager::ECSManager(GameContext& context, WorldInterface& world)
    : context_{context},
      systems_{make_unique<ServerEntitySystems>(context, world)},
      registry_{make_unique<EntityRegistry>(context, *systems_)} {};

ECSManager::~ECSManager() = default;

ServerEntitySystems& ECSManager::GetSystems() noexcept { return *systems_; }
const ServerEntitySystems& ECSManager::GetSystems() const noexcept {
  return *systems_;
}

EntityRegistry& ECSManager::GetRegistry() noexcept { return *registry_; }
const EntityRegistry& ECSManager::GetRegistry() const noexcept {
  return *registry_;
}

void ECSManager::Tick() {
  // Execute systems in order
  systems_->GetConcreteTickStateSystem().Tick();
  systems_->GetConcreteTickStateSystem().Commit();

  systems_->GetGravitySystem().Tick();
  systems_->GetConcreteTransformSystem().Tick();
  systems_->GetCollisionSystem().Tick();
  systems_->GetBlockPlaceSystem().Tick();
  systems_->GetBlockSystem().Tick();


  // Commit all component updates
  systems_->CommitAll();
  
  // Commit registry
  registry_->Commit();
}

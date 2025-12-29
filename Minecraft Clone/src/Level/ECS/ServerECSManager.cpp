#include "Level/ECS/ServerECSManager.h"

#include "Level/ECS/EntityRegistry.h"
#include "Level/ECS/ServerEntitySystems.h"
#include "Level/ECS/Systems/BlockPlaceSystem.h"
#include "Level/ECS/Systems/BlockSystem.h"
#include "Level/ECS/Systems/CollisionSystem.h"
#include "Level/ECS/Systems/GravitySystem.h"
#include "Level/ECS/Systems/TickStateSystem.h"
#include "Level/ECS/Systems/TransformSystem.h"
#include "Level/World/WorldUpdater.h"

using std::make_unique;

ServerECSManager::ServerECSManager(GameContext& context, WorldInterface& world)
    : context_{context},
      systems_{make_unique<ServerEntitySystems>(context, world)},
      registry_{make_unique<EntityRegistry>(context, *systems_)} {}

ServerECSManager::~ServerECSManager() = default;

void ServerECSManager::SetWorldUpdater(WorldUpdater* updater) {
  world_updater_ = updater;
  systems_->SetWorldUpdater(updater);
}

// IECSManager interface
IEntitySystems& ServerECSManager::GetSystems() {
  return *systems_;
}

const IEntitySystems& ServerECSManager::GetSystems() const {
  return *systems_;
}

// Server-specific access to concrete types
ServerEntitySystems& ServerECSManager::GetConcreteSystems() noexcept {
  return *systems_;
}

const ServerEntitySystems& ServerECSManager::GetConcreteSystems() const noexcept {
  return *systems_;
}

EntityRegistry& ServerECSManager::GetRegistry() noexcept {
  return *registry_;
}

const EntityRegistry& ServerECSManager::GetRegistry() const noexcept {
  return *registry_;
}

void ServerECSManager::CommitAll() {
  systems_->CommitAll();
}

void ServerECSManager::Tick() {
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


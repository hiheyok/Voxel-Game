#include "Level/ECS/ECSManager.h"

#include "Level/ECS/EntityRegistry.h"
#include "Level/ECS/EntitySystems.h"
#include "Level/ECS/Systems/BlockPlaceSystem.h"
#include "Level/ECS/Systems/CollisionSystem.h"
#include "Level/ECS/Systems/GravitySystem.h"
#include "Level/ECS/Systems/TransformSystem.h"

using std::make_unique;

ECSManager::ECSManager(GameContext& context, WorldInterface& world)
    : context_{context},
      systems_{make_unique<EntitySystems>(context, world)},
      registry_{make_unique<EntityRegistry>(context, *systems_)} {};

ECSManager::~ECSManager() = default;

EntitySystems& ECSManager::GetSystems() noexcept { return *systems_; }
const EntitySystems& ECSManager::GetSystems() const noexcept {
  return *systems_;
}

EntityRegistry& ECSManager::GetRegistry() noexcept { return *registry_; }
const EntityRegistry& ECSManager::GetRegistry() const noexcept {
  return *registry_;
}

void ECSManager::Tick() {
  // Execute systems in order
  systems_->GetGravitySystem().Tick();
  systems_->GetTransformSystem().Tick();
  systems_->GetCollisionSystem().Tick();
  systems_->GetBlockPlaceSystem().Tick();
  
  // Commit all component updates
  systems_->CommitAll();
}

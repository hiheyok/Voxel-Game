#include "Level/ECS/ServerEntitySystems.h"

#include "Level/ECS/Systems/BlockPlaceSystem.h"
#include "Level/ECS/Systems/BlockSystem.h"
#include "Level/ECS/Systems/CollisionSystem.h"
#include "Level/ECS/Systems/GravitySystem.h"
#include "Level/ECS/Systems/ITickStateSystem.h"
#include "Level/ECS/Systems/ITransformSystem.h"
#include "Level/ECS/Systems/TickStateSystem.h"
#include "Level/ECS/Systems/TransformSystem.h"
#include "Level/World/WorldUpdater.h"

using std::make_unique;

ServerEntitySystems::ServerEntitySystems(GameContext& context,
                                         WorldInterface& world)
    : context_{context},
      world_{world},
      transform_system_{make_unique<TransformSystem>(context, world, *this)},
      gravity_system_{make_unique<GravitySystem>(context, world, *this)},
      collision_system_{make_unique<CollisionSystem>(context, world, *this)},
      block_system_{make_unique<BlockSystem>(context, world, *this)},
      block_place_system_{make_unique<BlockPlaceSystem>(context, world, *this)},
      tick_state_system_{make_unique<TickStateSystem>(context, world, *this)} {}

ServerEntitySystems::~ServerEntitySystems() = default;

void ServerEntitySystems::SetWorldUpdater(WorldUpdater* updater) {
  if (block_place_system_) {
    block_place_system_->SetWorldUpdater(updater);
  }
}

// IEntitySystems interface implementations
ITransformSystem& ServerEntitySystems::GetTransformSystem() {
  return *transform_system_;
}

const ITransformSystem& ServerEntitySystems::GetTransformSystem() const {
  return *transform_system_;
}

ITickStateSystem& ServerEntitySystems::GetTickStateSystem() {
  return *tick_state_system_;
}

const ITickStateSystem& ServerEntitySystems::GetTickStateSystem() const {
  return *tick_state_system_;
}

// Server-specific concrete type accessors
TransformSystem& ServerEntitySystems::GetConcreteTransformSystem() {
  return *transform_system_;
}

TickStateSystem& ServerEntitySystems::GetConcreteTickStateSystem() {
  return *tick_state_system_;
}

GravitySystem& ServerEntitySystems::GetGravitySystem() {
  return *gravity_system_;
}

CollisionSystem& ServerEntitySystems::GetCollisionSystem() {
  return *collision_system_;
}

BlockSystem& ServerEntitySystems::GetBlockSystem() { return *block_system_; }

BlockPlaceSystem& ServerEntitySystems::GetBlockPlaceSystem() {
  return *block_place_system_;
}

void ServerEntitySystems::CommitAll() {
  transform_system_->Commit();
  gravity_system_->Commit();
  collision_system_->Commit();
  block_system_->Commit();
  block_place_system_->Commit();
  tick_state_system_->Commit();
}

void ServerEntitySystems::Reset() {
  tick_state_system_ = make_unique<TickStateSystem>(context_, world_, *this);
  transform_system_ = make_unique<TransformSystem>(context_, world_, *this);
  gravity_system_ = make_unique<GravitySystem>(context_, world_, *this);
  collision_system_ = make_unique<CollisionSystem>(context_, world_, *this);
  block_system_ = make_unique<BlockSystem>(context_, world_, *this);
  block_place_system_ = make_unique<BlockPlaceSystem>(context_, world_, *this);
}

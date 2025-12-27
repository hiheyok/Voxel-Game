#include "Level/ECS/EntitySystems.h"

#include "Level/ECS/Systems/BlockPlaceSystem.h"
#include "Level/ECS/Systems/BlockSystem.h"
#include "Level/ECS/Systems/CollisionSystem.h"
#include "Level/ECS/Systems/GravitySystem.h"
#include "Level/ECS/Systems/TransformSystem.h"

using std::make_unique;

EntitySystems::EntitySystems(GameContext& context, WorldInterface& world)
    : context_{context},
      world_{world},
      transform_system_{make_unique<TransformSystem>(context, world, *this)},
      gravity_system_{make_unique<GravitySystem>(context, world, *this)},
      collision_system_{make_unique<CollisionSystem>(context, world, *this)},
      block_system_{make_unique<BlockSystem>(context, world, *this)},
      block_place_system_{make_unique<BlockPlaceSystem>(context, world, *this)} {}

EntitySystems::~EntitySystems() = default;

TransformSystem& EntitySystems::GetTransformSystem() {
  return *transform_system_;
}

GravitySystem& EntitySystems::GetGravitySystem() { return *gravity_system_; }

CollisionSystem& EntitySystems::GetCollisionSystem() {
  return *collision_system_;
}

BlockSystem& EntitySystems::GetBlockSystem() { return *block_system_; }

BlockPlaceSystem& EntitySystems::GetBlockPlaceSystem() {
  return *block_place_system_;
}

void EntitySystems::CommitAll() {
  transform_system_->Commit();
  gravity_system_->Commit();
  collision_system_->Commit();
  block_system_->Commit();
  block_place_system_->Commit();
}

void EntitySystems::Reset() {
  transform_system_ = make_unique<TransformSystem>(context_, world_, *this);
  gravity_system_ = make_unique<GravitySystem>(context_, world_, *this);
  collision_system_ = make_unique<CollisionSystem>(context_, world_, *this);
  block_system_ = make_unique<BlockSystem>(context_, world_, *this);
  block_place_system_ = make_unique<BlockPlaceSystem>(context_, world_, *this);
}

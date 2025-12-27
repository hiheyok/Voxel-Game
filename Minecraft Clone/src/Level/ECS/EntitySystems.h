#pragma once

#include <memory>

class TransformSystem;
class GravitySystem;
class CollisionSystem;
class BlockSystem;
class BlockPlaceSystem;
class WorldInterface;
class GameContext;

class EntitySystems {
 public:
  explicit EntitySystems(GameContext& context, WorldInterface& world);
  ~EntitySystems();

  TransformSystem& GetTransformSystem();
  GravitySystem& GetGravitySystem();
  CollisionSystem& GetCollisionSystem();
  BlockSystem& GetBlockSystem();
  BlockPlaceSystem& GetBlockPlaceSystem();

  void CommitAll();
  void Reset();

 private:
  GameContext& context_;
  WorldInterface& world_;
  
  std::unique_ptr<TransformSystem> transform_system_;
  std::unique_ptr<GravitySystem> gravity_system_;
  std::unique_ptr<CollisionSystem> collision_system_;
  std::unique_ptr<BlockSystem> block_system_;
  std::unique_ptr<BlockPlaceSystem> block_place_system_;
};

#pragma once

#include <memory>

#include "Level/ECS/IEntitySystems.h"

class TransformSystem;
class GravitySystem;
class CollisionSystem;
class BlockSystem;
class BlockPlaceSystem;
class TickStateSystem;
class WorldInterface;
class WorldUpdater;
class GameContext;

/**
 * Server-side entity systems container.
 * Contains all systems needed for full server simulation.
 */
class ServerEntitySystems : public IEntitySystems {
 public:
  explicit ServerEntitySystems(GameContext& context, WorldInterface& world);
  ~ServerEntitySystems();

  // Set WorldUpdater for systems that need it (called before first tick)
  void SetWorldUpdater(WorldUpdater* updater);

  // IEntitySystems interface
  ITransformSystem& GetTransformSystem() override;
  const ITransformSystem& GetTransformSystem() const override;
  ITickStateSystem& GetTickStateSystem() override;
  const ITickStateSystem& GetTickStateSystem() const override;
  void CommitAll() override;

  // Server-specific systems
  GravitySystem& GetGravitySystem();
  CollisionSystem& GetCollisionSystem();
  BlockSystem& GetBlockSystem();
  BlockPlaceSystem& GetBlockPlaceSystem();
  
  // Server-specific access to concrete types
  TransformSystem& GetConcreteTransformSystem();
  TickStateSystem& GetConcreteTickStateSystem();

  void Reset();

 private:
  GameContext& context_;
  WorldInterface& world_;
  
  std::unique_ptr<TransformSystem> transform_system_;
  std::unique_ptr<GravitySystem> gravity_system_;
  std::unique_ptr<CollisionSystem> collision_system_;
  std::unique_ptr<BlockSystem> block_system_;
  std::unique_ptr<BlockPlaceSystem> block_place_system_;
  std::unique_ptr<TickStateSystem> tick_state_system_;
};

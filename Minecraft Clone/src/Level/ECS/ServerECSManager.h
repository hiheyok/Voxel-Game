#pragma once

#include <memory>

#include "Level/ECS/IECSManager.h"

class GameContext;
class ServerEntitySystems;
class EntityRegistry;
class WorldInterface;
class WorldUpdater;
class IEntitySystems;

/**
 * Server-side ECS manager with full entity management capabilities.
 * Includes EntityRegistry for creating/destroying entities and all systems.
 */
class ServerECSManager : public IECSManager {
 public:
  explicit ServerECSManager(GameContext& context, WorldInterface& world);
  ~ServerECSManager();

  // Set WorldUpdater after construction (called by Dimension before first tick)
  void SetWorldUpdater(WorldUpdater* updater);

  // IECSManager interface
  IEntitySystems& GetSystems() override;
  const IEntitySystems& GetSystems() const override;
  void CommitAll() override;

  // Server-specific access to concrete ServerEntitySystems
  ServerEntitySystems& GetConcreteSystems() noexcept;
  const ServerEntitySystems& GetConcreteSystems() const noexcept;
  
  EntityRegistry& GetRegistry() noexcept;
  const EntityRegistry& GetRegistry() const noexcept;

  void Tick();

 private:
  GameContext& context_;
  WorldUpdater* world_updater_ = nullptr;

  std::unique_ptr<ServerEntitySystems> systems_;
  std::unique_ptr<EntityRegistry> registry_;
};


#pragma once

#include <memory>

#include "Core/Typenames.h"
#include "Level/ECS/EntityType.h"
#include "Level/ECS/IECSManager.h"

class GameContext;
class WorldInterface;
class IEntitySystems;
class ClientEntitySystems;

/**
 * Client-side ECS manager with read-only capabilities.
 * Delegates system storage to ClientEntitySystems.
 */
class ClientECSManager : public IECSManager {
 public:
  explicit ClientECSManager(GameContext& context, WorldInterface& world);
  ~ClientECSManager();

  // IECSManager interface
  IEntitySystems& GetSystems() override;
  const IEntitySystems& GetSystems() const override;
  void CommitAll() override;

  // Client-specific access to concrete type
  ClientEntitySystems& GetConcreteSystems() noexcept;
  const ClientEntitySystems& GetConcreteSystems() const noexcept;

 private:
  std::unique_ptr<ClientEntitySystems> systems_;
};

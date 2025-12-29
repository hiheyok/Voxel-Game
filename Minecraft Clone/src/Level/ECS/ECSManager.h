#pragma once

#include <memory>

class GameContext;
class ServerEntitySystems;
class EntityRegistry;
class WorldInterface;

class ECSManager {
 public:
  explicit ECSManager(GameContext& context, WorldInterface& world);
  ~ECSManager();
  ServerEntitySystems& GetSystems() noexcept;
  const ServerEntitySystems& GetSystems() const noexcept;

  EntityRegistry& GetRegistry() noexcept;
  const EntityRegistry& GetRegistry() const noexcept;

  void Tick();

 private:
  GameContext& context_;

  std::unique_ptr<ServerEntitySystems> systems_;
  std::unique_ptr<EntityRegistry> registry_;
};

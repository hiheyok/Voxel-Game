#pragma once

#include <memory>

class GameContext;
class EntitySystems;
class EntityRegistry;
class WorldInterface;

class ECSManager {
 public:
  explicit ECSManager(GameContext& context, WorldInterface& world);
  ~ECSManager();
  EntitySystems& GetSystems() noexcept;
  const EntitySystems& GetSystems() const noexcept;

  EntityRegistry& GetRegistry() noexcept;
  const EntityRegistry& GetRegistry() const noexcept;

  void Tick();

 private:
  GameContext& context_;

  std::unique_ptr<EntitySystems> systems_;
  std::unique_ptr<EntityRegistry> registry_;
};

#pragma once

#include <stack>
#include <vector>

#include "Core/Typenames.h"
#include "Level/ECS/ComponentMap.h"
#include "Utils/Assert.h"

class WorldInterface;
class GameContext;
class EntitySystems;

template <class ComponentType>
class EntitySystem {
 public:
  EntitySystem(GameContext& context, WorldInterface& world, EntitySystems& entity_systems);

  EntitySystem(const EntitySystem&) = delete;
  EntitySystem(EntitySystem&&) = delete;

  ComponentType& AddComponent(EntityUUID uuid);
  bool HasComponent(EntityUUID) const noexcept;
  const ComponentType& GetComponent(EntityUUID uuid) const noexcept;
  const ComponentMap<ComponentType>& GetComponentMap() const noexcept;
  void RemoveComponent(EntityUUID uuid);
  void ReplaceComponent(EntityUUID uuid, const ComponentType& component);

  void Commit();

  virtual void Tick() = 0;

 protected:
  virtual void OnCommit();

  GameContext& context_;
  EntitySystems& entity_systems_;

  ComponentMap<ComponentType> components_active_;
  ComponentMap<ComponentType> components_updated_;

  WorldInterface& world_;

 private:
  std::vector<EntityUUID> delete_queue_;
};

template <class ComponentType>
EntitySystem<ComponentType>::EntitySystem(GameContext& context,
                                          WorldInterface& world,
                                          EntitySystems& entity_systems)
    : context_{context}, world_{world}, entity_systems_{entity_systems} {}


template <class ComponentType>
ComponentType& EntitySystem<ComponentType>::AddComponent(EntityUUID uuid) {
  GAME_ASSERT(!HasComponent(uuid), "Entity already has this component!");
  return components_updated_.AddComponent(uuid);
}

template <class ComponentType>
bool EntitySystem<ComponentType>::HasComponent(EntityUUID uuid) const noexcept {
  return components_active_.HasComponent(uuid);
}

template <class ComponentType>
const ComponentType& EntitySystem<ComponentType>::GetComponent(
    EntityUUID uuid) const noexcept {
  return components_active_.GetComponent(uuid);
}

template <class ComponentType>
const ComponentMap<ComponentType>&
EntitySystem<ComponentType>::GetComponentMap() const noexcept {
  return components_active_;
}

template <class ComponentType>
void EntitySystem<ComponentType>::RemoveComponent(EntityUUID uuid) {
  delete_queue_.push_back(uuid);
}

template <class ComponentType>
void EntitySystem<ComponentType>::Commit() {
  // Inefficient need to change later

  for (auto uuid : delete_queue_) {
    if (components_updated_.HasComponent(uuid)) {
      components_updated_.RemoveComponent(uuid);
    }
  }

  components_active_ = components_updated_;

  OnCommit();
}

template <class ComponentType>
void EntitySystem<ComponentType>::OnCommit() {}

template <class ComponentType>
void EntitySystem<ComponentType>::ReplaceComponent(EntityUUID uuid,
                                                  const ComponentType& component) {
  components_updated_.ReplaceComponent(uuid, component);
}

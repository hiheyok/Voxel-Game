#pragma once

#include "Level/ECS/ComponentMap.h"
#include "Level/ECS/Components/TransformComponent.h"
#include "Level/ECS/Systems/ITransformSystem.h"

/**
 * Client-side transform system implementation.
 * Read-only system that receives updates from the server.
 */
class ClientTransformSystem : public ITransformSystem {
 public:
  const ComponentMap<TransformComponent>& GetComponentMap() const override {
    return components_;
  }

  void ReplaceComponent(EntityUUID uuid,
                        const TransformComponent& component) override {
    components_.ReplaceComponent(uuid, component);
  }

 private:
  ComponentMap<TransformComponent> components_;
};

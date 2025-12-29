#pragma once

#include "Level/ECS/ComponentMap.h"
#include "Level/ECS/Components/TickStateComponent.h"
#include "Level/ECS/Systems/ITickStateSystem.h"

/**
 * Client-side tick state system implementation.
 * Read-only system that receives updates from the server.
 */
class ClientTickStateSystem : public ITickStateSystem {
 public:
  bool HasComponent(EntityUUID uuid) const override {
    return components_.HasComponent(uuid);
  }

  bool IsActive(EntityUUID uuid) const noexcept override {
    if (!components_.HasComponent(uuid)) return false;
    return components_.GetComponent(uuid).state_ ==
           TickStateComponent::State::kActive;
  }

 private:
  ComponentMap<TickStateComponent> components_;
};

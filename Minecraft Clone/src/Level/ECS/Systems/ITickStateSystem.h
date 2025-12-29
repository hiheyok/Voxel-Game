#pragma once

#include "Core/Typenames.h"
#include "Level/ECS/Components/TickStateComponent.h"

template <class ComponentType>
class ComponentMap;

/**
 * Base interface for tick state system - used by both server and client ECS.
 * Provides read-only query access to tick state.
 * Note: Commit() is an implementation detail, not part of the interface.
 */
class ITickStateSystem {
 public:
  virtual ~ITickStateSystem() = default;
  
  virtual bool HasComponent(EntityUUID uuid) const = 0;
  virtual bool IsActive(EntityUUID uuid) const = 0;
};

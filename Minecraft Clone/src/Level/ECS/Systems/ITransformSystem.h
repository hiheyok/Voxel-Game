#pragma once

#include "Core/Typenames.h"
#include "Level/ECS/Components/TransformComponent.h"

template <class ComponentType>
class ComponentMap;

/**
 * Base interface for transform system - used by both server and client ECS.
 * Provides read/write access to transform components.
 * Note: Commit() is an implementation detail, not part of the interface.
 */
class ITransformSystem {
 public:
  virtual ~ITransformSystem() = default;
  
  virtual const ComponentMap<TransformComponent>& GetComponentMap() const = 0;
  virtual void ReplaceComponent(EntityUUID uuid, const TransformComponent& component) = 0;
};

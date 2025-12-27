// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <utility>
#include <vector>

#include "Core/Typenames.h"
#include "Level/ECS/EntityType.h"

struct EntityProperty;

struct EntityRenderCache {  // stores all entity to be rendered
  void AddEntity(const EntityProperty& entity);
  void RemoveEntity(EntityUUID entityUUID);
  FastHashMap<EntityType, std::vector<EntityProperty>>&
  GetEntitiesTypeSeparated();

  FastHashMap<EntityUUID, std::pair<size_t, EntityType>> entity_idx_;
  FastHashMap<EntityType, std::vector<EntityProperty>> entity_separated_;
};

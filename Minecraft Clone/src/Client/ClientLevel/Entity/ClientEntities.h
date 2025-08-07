// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <utility>
#include <vector>

#include "Core/Typenames.h"
#include "Level/Entity/Entity.h"

class GameContext;

class ClientEntities {
 public:
  explicit ClientEntities(GameContext&);
  ~ClientEntities();

  void InsertEntity(const EntityProperty& entityProperty);
  void UpdateEntity(const EntityProperty& entityProperty);
  EntityProperty GetEntity(const EntityUUID& uuid) const;
  void RemoveEntity(const EntityUUID& uuid);
  std::vector<EntityProperty> GetAllEntities() const;
  std::vector<EntityProperty> GetChangedEntities();
  std::vector<EntityUUID> GetRemovedEntitiesUUID();
  size_t GetEntityCount() const;
  size_t GetChangedEntityCount() const;

 private:
  GameContext& context_;

  FastHashMap<EntityUUID, size_t> changed_entities_;
  std::vector<EntityProperty> changed_entities_list_;
  FastHashMap<EntityUUID, size_t> removed_entities_;
  std::vector<EntityUUID> removed_entities_list_;

  FastHashMap<EntityUUID, size_t> entity_idx_;
  std::vector<EntityProperty> all_entities_;
};

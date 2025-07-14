// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>
#include <mutex>
#include <vector>

#include "Core/Typenames.h"
class Dimension;

struct Entity;
struct EntityProperty;

class EntityContainer {  // Manages all entities in world
 public:
  EntityContainer();
  ~EntityContainer();
  EntityUUID AddEntity(std::unique_ptr<Entity> e);
  void RemoveEntity(EntityUUID entityId);

  int GetEntityCount() const;

  std::vector<EntityProperty> GetSpawnedEntities();
  std::vector<EntityProperty> GetUpdatedEntities();
  std::vector<EntityUUID> GetRemovedEntities();

  Entity* GetEntity(EntityUUID entityId) const;
  void Tick(Dimension* dimension);

  void ResetState();

 private:
  FastHashMap<EntityUUID, size_t> entities_idx_;
  std::vector<std::unique_ptr<Entity>> entities_;
  FastHashSet<EntityUUID> removed_entity_;
  FastHashSet<EntityUUID> spawned_entity_;

  std::mutex entity_lock_;
  EntityUUID unique_id_ = 0;
  int entity_count_ = 0;
};

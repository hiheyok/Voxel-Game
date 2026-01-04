// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Container/EntityContainer.h"

#include <cstddef>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Core/Typenames.h"
#include "Level/Entity/Entity.h"
#include "Level/Entity/Properties/EntityProperties.h"

using std::lock_guard;
using std::move;
using std::mutex;
using std::unique_ptr;
using std::vector;

EntityContainer::EntityContainer() = default;
EntityContainer::~EntityContainer() = default;

EntityUUID EntityContainer::AddEntity(unique_ptr<Entity> e) {
  e->properties_.entity_uuid_ = unique_id_;
  spawned_entity_.insert(unique_id_);
  entities_idx_.emplace(unique_id_, entities_.size());
  entities_.push_back(move(e));
  unique_id_++;
  entity_count_++;
  return unique_id_ - 1;
}

int EntityContainer::GetEntityCount() const { return entity_count_; }

vector<EntityProperty> EntityContainer::GetSpawnedEntities() {
  lock_guard<mutex> lock{entity_lock_};
  vector<EntityProperty> out;

  for (const auto& entity : spawned_entity_) {
    out.emplace_back(GetEntity(entity)->properties_);
  }
  return out;
}

vector<EntityProperty>
EntityContainer::GetUpdatedEntities() {  // change this to past on a vector
                                         // later
  lock_guard<mutex> lock{entity_lock_};
  vector<EntityProperty> out;
  out.reserve(entities_idx_.size());

  for (auto& entity : entities_) {
    if (entity->is_dirty_) {
      out.emplace_back(entity->properties_);
    }
  }

  return out;
}

vector<EntityUUID> EntityContainer::GetRemovedEntities() {
  return vector<EntityUUID>(removed_entity_.begin(), removed_entity_.end());
}

void EntityContainer::RemoveEntity(EntityUUID entityId) {
  auto it = entities_idx_.find(entityId);
  if (it == entities_idx_.end()) {
    throw std::logic_error(
        "EntityContainer::RemoveEntity - Tried to remove entity that doesn't "
        "exist!");
  }
  size_t removeIdx = it->second;
  entities_idx_[entities_.back()->properties_.entity_uuid_] = removeIdx;
  std::swap(entities_.back(), entities_[removeIdx]);
  entities_idx_.erase(it);
  entities_.pop_back();
  lock_guard<mutex> lock{entity_lock_};
  removed_entity_.emplace(entityId);
  entity_count_--;
}

Entity* EntityContainer::GetEntity(EntityUUID entityId) const {
  const auto& it = entities_idx_.find(entityId);
  if (it == entities_idx_.end()) {
    return nullptr;
  } else {
    return entities_[it->second].get();
  }
}

void EntityContainer::Tick(Dimension* dimension) {
  for (auto& entity : entities_) {
    entity->Tick(dimension);
  }
}

void EntityContainer::ResetState() {
  spawned_entity_.clear();
  removed_entity_.clear();

  for (auto& entity : entities_) {
    entity->is_dirty_ = false;
  }
}

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientLevel/Entity/ClientEntities.h"

#include <cstddef>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Typenames.h"
#include "Level/Entity/Properties/EntityProperties.h"
#include "Utils/Assert.h"

using std::vector;

ClientEntities::ClientEntities(GameContext& context) : context_{context} {}
ClientEntities::~ClientEntities() = default;

void ClientEntities::InsertEntity(const EntityProperty& entityProperty) {
  const EntityUUID& uuid = entityProperty.entity_uuid_;

  GAME_ASSERT(!entity_idx_.count(uuid),
              "Attempted to insert already existing entity");

  changed_entities_.emplace(uuid, changed_entities_list_.size());
  changed_entities_list_.push_back(entityProperty);

  all_entities_.push_back(entityProperty);
  entity_idx_[uuid] = all_entities_.size() - 1;

  // Delete entity from removed if it is in there

  if (removed_entities_.count(uuid)) {
    size_t idx = removed_entities_[uuid];
    removed_entities_[removed_entities_list_.back()] = idx;
    std::swap(removed_entities_list_.back(), removed_entities_list_[idx]);
    removed_entities_.erase(entityProperty.entity_uuid_);
    removed_entities_list_.pop_back();
  }
}

void ClientEntities::UpdateEntity(const EntityProperty& entityProperty) {
  const EntityUUID& uuid = entityProperty.entity_uuid_;
  auto it = entity_idx_.find(uuid);
  GAME_ASSERT(it != entity_idx_.end(),
              "Attempted to change nonexistent entity");
  all_entities_[it->second] = entityProperty;
  auto changedIt = changed_entities_.find(uuid);
  if (changedIt != changed_entities_.end()) {
    size_t idx = changedIt->second;
    changed_entities_list_[idx] = entityProperty;
  } else {
    changed_entities_.emplace(uuid, changed_entities_list_.size());
    changed_entities_list_.push_back(entityProperty);
  }
}

EntityProperty ClientEntities::GetEntity(const EntityUUID& uuid) const {
  const auto& it = entity_idx_.find(uuid);
  GAME_ASSERT(it != entity_idx_.end(), "Attempted to get nonexistent entity");
  return all_entities_[it->second];
}

void ClientEntities::RemoveEntity(const EntityUUID& uuid) {
  auto it = entity_idx_.find(uuid);
  GAME_ASSERT(it != entity_idx_.end(),
              "Attempted to remove nonexistent entity");

  size_t idx = it->second;
  entity_idx_[all_entities_.back().entity_uuid_] = idx;
  std::swap(all_entities_[idx], all_entities_.back());
  entity_idx_.erase(it);
  all_entities_.pop_back();

  auto changedIt = changed_entities_.find(uuid);
  if (changedIt != changed_entities_.end()) {
    changed_entities_.erase(changedIt);
  }

  removed_entities_.emplace(uuid, removed_entities_list_.size());
  removed_entities_list_.push_back(uuid);
}

vector<EntityProperty> ClientEntities::GetAllEntities() const {
  return all_entities_;
}

vector<EntityProperty> ClientEntities::GetChangedEntities() {
  vector<EntityProperty> out = move(changed_entities_list_);
  changed_entities_.clear();
  changed_entities_list_.clear();
  return out;
}

vector<EntityUUID> ClientEntities::GetRemovedEntitiesUUID() {
  vector<EntityUUID> out = move(removed_entities_list_);
  removed_entities_.clear();
  removed_entities_list_.clear();
  return out;
}

size_t ClientEntities::GetEntityCount() const { return all_entities_.size(); }

size_t ClientEntities::GetChangedEntityCount() const {
  return changed_entities_.size();
}
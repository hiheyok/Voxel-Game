#pragma once
#include "Core/Typenames.h"
#include "Level/Entity/Entity.h"

class ClientEntities {
public:

    void InsertEntity(const EntityProperty& entityProperty) {
        const EntityUUID& uuid = entityProperty.entity_uuid_;

        if (entities_.count(uuid)) {
            g_logger.LogError("ClientEntities::InsertEntity", "Tried to insert already existing entity.");
            return;
        }

        entities_[uuid] = entityProperty;
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

    void UpdateEntity(const EntityProperty& entityProperty) {
        const EntityUUID& uuid = entityProperty.entity_uuid_;

        if (!entities_.count(uuid)) {
            g_logger.LogError("ClientEntities::ChangeEntity", "Tried to change entity that doesn't exist.");
            return;
        }
        entities_[uuid] = entityProperty;
        if (changed_entities_.contains(uuid)) {
            size_t idx = changed_entities_[uuid];
            changed_entities_list_[idx] = entityProperty;
        }
        else {
            changed_entities_.emplace(uuid, changed_entities_list_.size());
            changed_entities_list_.push_back(entityProperty);
        }
        
    }

    EntityProperty GetEntity(const EntityUUID& uuid) const {
        if (!entities_.contains(uuid)) {
            g_logger.LogError("ClientEntities::GetEntity", "Tried to get entity that doesn't exist.");
        }

        const auto& it = entities_.find(uuid);
        return it->second;
    }

    void RemoveEntity(const EntityUUID& uuid) {
        if (!entities_.count(uuid)) {
            g_logger.LogError("ClientEntities::RemoveEntity", "Tried to remove entity that doesn't exist.");
        }

        entities_.erase(uuid);

        size_t idx = entity_idx_[uuid];
        entity_idx_[all_entities_.back().entity_uuid_] = idx;
        std::swap(all_entities_[idx], all_entities_.back());
        entity_idx_.erase(uuid);
        all_entities_.pop_back();

        if (changed_entities_.contains(uuid)) {
            changed_entities_.erase(uuid);
        }
        removed_entities_.emplace(uuid, removed_entities_list_.size());
        removed_entities_list_.push_back(uuid);
    }

    std::vector<EntityProperty> GetAllEntities() const {
        return all_entities_;
    }

    std::vector<EntityProperty> GetChangedEntities() {
        std::vector<EntityProperty> out = std::move(changed_entities_list_);
        changed_entities_.clear();
        changed_entities_list_.clear();
        return out;
    }

    std::vector<EntityUUID> GetRemovedEntitiesUUID() {
        std::vector<EntityUUID> out = std::move(removed_entities_list_);
        removed_entities_.clear();
        removed_entities_list_.clear();
        return out;
    }

    size_t GetEntityCount() const {
        return entities_.size();
    }

    size_t GetChangedEntityCount() const {
        return changed_entities_.size();
    }
private:
    FastHashMap<EntityUUID, EntityProperty> entities_;
    FastHashMap<EntityUUID, size_t> changed_entities_;
    std::vector<EntityProperty> changed_entities_list_;
    FastHashMap<EntityUUID, size_t> removed_entities_;
    std::vector<EntityUUID> removed_entities_list_;

    FastHashMap<EntityUUID, size_t> entity_idx_;
    std::vector<EntityProperty> all_entities_;
};
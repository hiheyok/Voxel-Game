#pragma once
#include "../../../Level/Typenames.h"
#include "../../../Level/Entity/Entity.h"

class ClientEntities {
private:
    FastHashMap<EntityUUID, EntityProperty> entities_;
    FastHashSet<EntityUUID> changed_entities_;
    FastHashSet<EntityUUID> removed_entities_;
public:

    void AddEntity(EntityProperty entityProperty) {
        if (entities_.count(entityProperty.entity_uuid_)) {
            g_logger.LogError("ClientEntities::AddEntity", "Tried to insert already existing entity.");
            return;
        }
        entities_[entityProperty.entity_uuid_] = entityProperty;
        changed_entities_.insert(entityProperty.entity_uuid_);

        // Delete entity from removed if it is in there

        if (removed_entities_.count(entityProperty.entity_uuid_)) {
            removed_entities_.erase(entityProperty.entity_uuid_);
        }
    }

    void ChangeEntity(EntityProperty entityProperty) {
        if (!entities_.count(entityProperty.entity_uuid_)) {
            g_logger.LogError("ClientEntities::ChangeEntity", "Tried to change entity that doesn't exist.");
            return;
        }
        entities_[entityProperty.entity_uuid_] = entityProperty;
        changed_entities_.insert(entityProperty.entity_uuid_);
    }

    EntityProperty GetEntity(EntityUUID uuid) const {
        if (!entities_.count(uuid)) {
            g_logger.LogError("ClientEntities::GetEntity", "Tried to get entity that doesn't exist.");
        }

        const auto& it = entities_.find(uuid);
        return it->second;
    }

    void RemoveEntity(EntityUUID uuid) {
        if (!entities_.count(uuid)) {
            g_logger.LogError("ClientEntities::RemoveEntity", "Tried to remove entity that doesn't exist.");
        }

        entities_.erase(uuid);
        if (changed_entities_.count(uuid)) {
            changed_entities_.erase(uuid);
        }
        removed_entities_.insert(uuid);
    }

    std::vector<EntityProperty> GetAllEntities() const {
        std::vector<EntityProperty> out;
        out.reserve(GetEntityCount());

        for (const auto& [uuid, entityProperty] : entities_) {
            out.push_back(entityProperty);
        }

        return out;
    }

    std::vector<EntityProperty> GetChangedEntities() {
        std::vector<EntityProperty> out;
        out.reserve(GetChangedEntityCount());

        for (EntityUUID uuid : changed_entities_) {
            out.push_back(entities_.find(uuid)->second);
        }

        changed_entities_.clear();

        return out;
    }

    std::vector<EntityUUID> GetRemovedEntitiesUUID() {
        std::vector<EntityUUID> out(removed_entities_.begin(), removed_entities_.end());
        removed_entities_.clear();
        return out;
    }

    size_t GetEntityCount() const {
        return entities_.size();
    }

    size_t GetChangedEntityCount() const {
        return changed_entities_.size();
    }
};
#pragma once
#include "../Entity/Entity.h"
#include "../../Utils/Containers/skaHashmap.h"
#include <mutex>

class EntityContainer { //Manages all entities in world
private:
    FastHashMap<EntityUUID, Entity*> entities_;
    FastHashSet<EntityUUID> removed_entity_;
    std::mutex entity_lock_;
    EntityUUID unique_id_ = 0;
    int entity_count_ = 0;
public:
    void AddEntities(Entity& e) {
        e.properties_.entity_uuid_ = unique_id_;
        entities_.emplace(unique_id_, &e);
        unique_id_++;
        entity_count_++;
    }

    void AddEntities(Entity* e) {
        e->properties_.entity_uuid_ = unique_id_;
        entities_.emplace(unique_id_, e);
        unique_id_++;
        entity_count_++;
    }

    int GetEntityCount() const {
        return entity_count_;
    }

    FastHashMap<EntityUUID, EntityProperty> ClientGetEntityUpdate() { //change this to past on a vector later
        FastHashMap<EntityUUID, EntityProperty> updatedData;

        for (auto& entity : entities_) {
            if (entity.second->is_dirty_)  {
                entity.second->is_dirty_ = false;
                updatedData.emplace(entity.second->properties_.entity_uuid_, entity.second->properties_);
            }
        }

        return updatedData;
    }

    void RemoveEntity(EntityUUID entityId) {
        delete entities_[entityId];
        entities_.erase(entityId);
        std::lock_guard<std::mutex> lock{ entity_lock_ };
        removed_entity_.emplace(entityId);
        entity_count_--;
    }

    FastHashSet<EntityUUID> getRemovedEntities() {
        std::lock_guard<std::mutex> lock{ entity_lock_ };
        return std::move(removed_entity_);
    }

    Entity* GetEntity(EntityUUID entityId) {
        if (entities_.count(entityId))
            return entities_[entityId];
        return nullptr;
    }

    void Tick() {
        for (auto& entity : entities_) {
            entity.second->Tick();
        }
    }

};
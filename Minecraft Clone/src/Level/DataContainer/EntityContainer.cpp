#include "EntityContainer.h"
#include "../Entity/Entity.h"
#include "../Entity/Properties/EntityProperties.h"

void EntityContainer::AddEntities(Entity& e) {
    e.properties_.entity_uuid_ = unique_id_;
    entities_.emplace(unique_id_, &e);
    unique_id_++;
    entity_count_++;
}

void EntityContainer::AddEntities(Entity* e) {
    e->properties_.entity_uuid_ = unique_id_;
    entities_.emplace(unique_id_, e);
    unique_id_++;
    entity_count_++;
}

int EntityContainer::GetEntityCount() const {
    return entity_count_;
}

FastHashMap<EntityUUID, EntityProperty> EntityContainer::ClientGetEntityUpdate() { //change this to past on a vector later
    FastHashMap<EntityUUID, EntityProperty> updatedData;
    for (auto& entity : entities_) {
        if (entity.second->is_dirty_) {
            entity.second->is_dirty_ = false;
            updatedData.emplace(entity.second->properties_.entity_uuid_, entity.second->properties_);
        }
    }

    return updatedData;
}

void EntityContainer::RemoveEntity(EntityUUID entityId) {
    delete entities_[entityId];
    entities_.erase(entityId);
    std::lock_guard<std::mutex> lock{ entity_lock_ };
    removed_entity_.emplace(entityId);
    entity_count_--;
}

FastHashSet<EntityUUID> EntityContainer::getRemovedEntities() {
    std::lock_guard<std::mutex> lock{ entity_lock_ };
    return std::move(removed_entity_);
}

Entity* EntityContainer::GetEntity(EntityUUID entityId) {
    if (entities_.count(entityId))
        return entities_[entityId];
    return nullptr;
}

void EntityContainer::Tick(Dimension* dimension) {
    for (auto& entity : entities_) {
        entity.second->Tick(dimension);
    }
}
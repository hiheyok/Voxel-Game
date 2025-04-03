#include "Level/Container/EntityContainer.h"
#include "Level/Entity/Entity.h"
#include "Level/Entity/Properties/EntityProperties.h"

EntityContainer::EntityContainer() = default;
EntityContainer::~EntityContainer() = default;

EntityUUID EntityContainer::AddEntity(std::unique_ptr<Entity> e) {
    e->properties_.entity_uuid_ = unique_id_;
    spawned_entity_.insert(unique_id_);
    entities_.emplace(unique_id_, std::move(e));
    unique_id_++;
    entity_count_++;
    return unique_id_ - 1;
}

int EntityContainer::GetEntityCount() const {
    return entity_count_;
}

std::vector<EntityProperty> EntityContainer::GetSpawnedEntities() {
    std::lock_guard<std::mutex> lock{ entity_lock_ };
    std::vector<EntityProperty> out;

    for (const auto& entity : spawned_entity_) {
        out.push_back(GetEntity(entity)->properties_);
    }
    spawned_entity_.clear();
    return out;
}

std::vector<EntityProperty> EntityContainer::GetUpdatedEntities() { //change this to past on a vector later
    std::lock_guard<std::mutex> lock{ entity_lock_ };
    std::vector<EntityProperty> out;

    for (auto& entity : entities_) {
        if (entity.second->is_dirty_) {
            entity.second->is_dirty_ = false;
            out.emplace_back(entity.second->properties_);
        }
    }

    return out;
}

std::vector<EntityUUID> EntityContainer::GetRemovedEntities() {
    std::lock_guard<std::mutex> lock{ entity_lock_ };
    std::vector<EntityUUID> out(removed_entity_.begin(), removed_entity_.end());
    removed_entity_.clear();
    return out;
}


void EntityContainer::RemoveEntity(EntityUUID entityId) {
    entities_.erase(entityId);
    std::lock_guard<std::mutex> lock{ entity_lock_ };
    removed_entity_.emplace(entityId);
    entity_count_--;
}


Entity* EntityContainer::GetEntity(EntityUUID entityId) const {
    const auto& it = entities_.find(entityId);
    if (it == entities_.end()) {
        return nullptr;
    }
    else {
        return it->second.get();
    }
}

void EntityContainer::Tick(Dimension* dimension) {
    for (auto& entity : entities_) {
        entity.second->Tick(dimension);
    }
}
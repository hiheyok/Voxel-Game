#include "RenderEngine/EntityRender/EntityRenderCache.h"
#include "Level/Entity/Entity.h"

void EntityRenderCache::AddEntity(const EntityProperty& entity) {
    const EntityTypeID& type = entity.type_;
    const EntityUUID& uuid = entity.entity_uuid_;

    auto it = entity_idx_.find(uuid);
    if (it != entity_idx_.end()) {
        size_t idx = it->second.first;
        entity_separated_[type][idx] = entity;
    } else {
        entity_idx_[uuid] = { entity_separated_[type].size(), type };
        entity_separated_[type].push_back(entity);
    }
}

void EntityRenderCache::RemoveEntity(EntityUUID entityUUID) {
    const auto& it = entity_idx_.find(entityUUID);

    if (it == entity_idx_.end()) {
        g_logger.LogError("EntityRenderCache::RemoveEntity", "Attempted to removed entity that doesn't exist!");
    }

    const auto& [idx, type] = it->second;
    EntityProperty e = entity_separated_[type][idx];
    auto& container = entity_separated_[e.type_];
    entity_idx_[container.back().entity_uuid_] = {idx , type};
    std::swap(container[idx], container.back());
    container.pop_back();
    entity_idx_.erase(it);
}

FastHashMap<EntityTypeID, std::vector<EntityProperty>>& EntityRenderCache::GetEntitiesTypeSeparated() {
    return entity_separated_;
}
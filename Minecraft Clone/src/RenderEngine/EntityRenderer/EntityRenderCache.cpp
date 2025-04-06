#include "RenderEngine/EntityRenderer/EntityRenderCache.h"
#include "Level/Entity/Entity.h"

void EntityRenderCache::AddEntity(const EntityProperty& entity) {
    entity_container_[entity.entity_uuid_] = entity;

    if (entity_idx_.contains(entity.entity_uuid_)) {
        size_t idx = entity_idx_[entity.entity_uuid_];
        entity_separated_[entity.type_][idx] = entity;
    } else {
        entity_idx_[entity.entity_uuid_] = entity_separated_[entity.type_].size();
        entity_separated_[entity.type_].push_back(entity);
    }
}

void EntityRenderCache::RemoveEntity(EntityUUID entityUUID) {
    if (!entity_idx_.contains(entityUUID)) {
        g_logger.LogError("EntityRenderCache::RemoveEntity", "Attempted to removed entity that doesn't exist!");
    }

    EntityProperty e = entity_container_[entityUUID];
    size_t idx = entity_idx_[entityUUID];
    auto& container = entity_separated_[e.type_];
    entity_idx_[container.back().entity_uuid_] = idx;
    std::swap(container[idx], container.back());
    container.pop_back();
    
    entity_container_.erase(entityUUID);
}

FastHashMap<EntityTypeID, std::vector<EntityProperty>>& EntityRenderCache::GetEntitiesTypeSeparated() {
    return entity_separated_;
}
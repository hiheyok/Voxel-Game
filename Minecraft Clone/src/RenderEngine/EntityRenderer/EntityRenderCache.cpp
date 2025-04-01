#include "RenderEngine/EntityRenderer/EntityRenderCache.h"
#include "Level/Entity/Entity.h"

void EntityRenderCache::AddEntity(EntityProperty& entity) {
    entity_container_[entity.entity_uuid_] = entity;
    entity_separated_[entity.type_][entity.entity_uuid_] = entity;
}

void EntityRenderCache::RemoveEntity(EntityUUID entityUUID) {
    EntityProperty e = entity_container_[entityUUID];

    entity_separated_[e.type_].erase(entityUUID);
    entity_container_.erase(entityUUID);
}

FastHashMap<EntityTypeID, FastHashMap<EntityUUID, EntityProperty>>& EntityRenderCache::GetEntitiesTypeSeparated() {
    return entity_separated_;
}
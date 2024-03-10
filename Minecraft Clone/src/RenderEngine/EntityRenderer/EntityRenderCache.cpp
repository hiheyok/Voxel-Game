#include "EntityRenderCache.h"

void EntityRenderCache::AddEntity(EntityProperty& entity) {
	
	EntityContainer[entity.EntityUUID] = entity;
	EntitySeparated[entity.Type][entity.EntityUUID] = entity;
	
}

void EntityRenderCache::RemoveEntity(EntityUUID EntityUUID) {
	EntityProperty e = EntityContainer[EntityUUID];

	EntitySeparated[e.Type].erase(EntityUUID);
	EntityContainer.erase(EntityUUID);
}

std::unordered_map<EntityTypeID, std::unordered_map<EntityUUID, EntityProperty>>& EntityRenderCache::GetEntitiesTypeSeparated() {
	return EntitySeparated;
}
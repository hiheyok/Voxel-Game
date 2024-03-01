#pragma once

#include <unordered_map>
#include "../../World/Entity/Entity.h"

struct EntityRenderCache { // stores all entity to be rendered

	std::unordered_map<EntityUUID, Entity> EntityContainer;

	std::unordered_map<EntityTypeID, std::unordered_map<EntityUUID, Entity>> EntitySeparated;

	void AddEntity(Entity entity) {
		EntityContainer[entity.EntityUUID] = entity;
		EntitySeparated[entity.Type][entity.EntityUUID] = entity;
	}

	void RemoveEntity(EntityUUID EntityUUID) {
		Entity e = EntityContainer[EntityUUID];

		EntitySeparated[e.Type].erase(EntityUUID);
		EntityContainer.erase(EntityUUID);
	}

	std::unordered_map<EntityTypeID, std::unordered_map<EntityUUID, Entity>>& GetEntitiesTypeSeparated() {
		return EntitySeparated;
	}

};
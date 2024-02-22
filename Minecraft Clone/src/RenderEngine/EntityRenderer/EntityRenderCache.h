#pragma once

#include <unordered_map>
#include "../../World/Entity/Entity.h"

struct EntityRenderCache { // stores all entity to be rendered

	std::unordered_map<EntityUUID, Entity> EntityContainer;

	void AddEntity(Entity entity) {
		EntityContainer[entity.EntityUUID] = entity;
	}

	void RemoveEntity(EntityUUID EntityUUID) {
		EntityContainer.erase(EntityUUID);
	}

};
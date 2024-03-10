#pragma once

#include <unordered_map>
#include "../../World/Entity/Entity.h"

struct EntityRenderCache { // stores all entity to be rendered

	std::unordered_map<EntityUUID, EntityProperty> EntityContainer;

	std::unordered_map<EntityTypeID, std::unordered_map<EntityUUID, EntityProperty>> EntitySeparated;

	void AddEntity(EntityProperty& entity);

	void RemoveEntity(EntityUUID EntityUUID);

	std::unordered_map<EntityTypeID, std::unordered_map<EntityUUID, EntityProperty>>& GetEntitiesTypeSeparated();

};
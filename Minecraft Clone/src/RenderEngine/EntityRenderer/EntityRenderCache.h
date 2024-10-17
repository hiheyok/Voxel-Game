#pragma once

#include <unordered_map>
#include "../../Level/Entity/Entity.h"
#include "../../Utils/Containers/skaHashmap.h"
struct EntityRenderCache { // stores all entity to be rendered

	ska::flat_hash_map<EntityUUID, EntityProperty> EntityContainer;

	ska::flat_hash_map<EntityTypeID, ska::flat_hash_map<EntityUUID, EntityProperty>> EntitySeparated;

	void AddEntity(EntityProperty& entity);

	void RemoveEntity(EntityUUID EntityUUID);

	ska::flat_hash_map<EntityTypeID, ska::flat_hash_map<EntityUUID, EntityProperty>>& GetEntitiesTypeSeparated();

};
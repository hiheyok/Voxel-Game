#pragma once

#include "../../Level/Entity/Entity.h"
#include "../../Utils/Containers/skaHashmap.h"
struct EntityRenderCache { // stores all entity to be rendered

	FastHashMap<EntityUUID, EntityProperty> EntityContainer;

	FastHashMap<EntityTypeID, FastHashMap<EntityUUID, EntityProperty>> EntitySeparated;

	void AddEntity(EntityProperty& entity);

	void RemoveEntity(EntityUUID EntityUUID);

	FastHashMap<EntityTypeID, FastHashMap<EntityUUID, EntityProperty>>& GetEntitiesTypeSeparated();

};
#pragma once

#include "../../Level/Entity/Entity.h"
#include "../../Utils/Containers/skaHashmap.h"
struct EntityRenderCache { // stores all entity to be rendered

    FastHashMap<EntityUUID, EntityProperty> entity_container_;

    FastHashMap<EntityTypeID, FastHashMap<EntityUUID, EntityProperty>> entity_separated_;

    void AddEntity(EntityProperty& entity);

    void RemoveEntity(EntityUUID entityUUID);

    FastHashMap<EntityTypeID, FastHashMap<EntityUUID, EntityProperty>>& GetEntitiesTypeSeparated();

};
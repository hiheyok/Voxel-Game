#pragma once

#include "Core/Typenames.h"

struct EntityProperty;

struct EntityRenderCache { // stores all entity to be rendered
    void AddEntity(const EntityProperty& entity);
    void RemoveEntity(EntityUUID entityUUID);
    FastHashMap<EntityTypeID, FastHashMap<EntityUUID, EntityProperty>>& GetEntitiesTypeSeparated();

    FastHashMap<EntityUUID, EntityProperty> entity_container_;
    FastHashMap<EntityTypeID, FastHashMap<EntityUUID, EntityProperty>> entity_separated_;
};
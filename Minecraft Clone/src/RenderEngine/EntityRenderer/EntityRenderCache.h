#pragma once

#include "Core/Typenames.h"

struct EntityProperty;

struct EntityRenderCache { // stores all entity to be rendered
    void AddEntity(const EntityProperty& entity);
    void RemoveEntity(EntityUUID entityUUID);
    FastHashMap<EntityTypeID, std::vector<EntityProperty>>& GetEntitiesTypeSeparated();

    FastHashMap<EntityUUID, EntityProperty> entity_container_;
    FastHashMap<EntityTypeID, std::vector<EntityProperty>> entity_separated_;
    FastHashMap<EntityUUID, size_t> entity_idx_;
};
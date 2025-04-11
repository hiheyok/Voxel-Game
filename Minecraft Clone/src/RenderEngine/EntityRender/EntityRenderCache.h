#pragma once

#include "Core/Typenames.h"

struct EntityProperty;

struct EntityRenderCache { // stores all entity to be rendered
    void AddEntity(const EntityProperty& entity);
    void RemoveEntity(EntityUUID entityUUID);
    FastHashMap<EntityTypeID, std::vector<EntityProperty>>& GetEntitiesTypeSeparated();

    FastHashMap<EntityUUID, std::pair<size_t, EntityTypeID>> entity_idx_;
    FastHashMap<EntityTypeID, std::vector<EntityProperty>> entity_separated_;
};
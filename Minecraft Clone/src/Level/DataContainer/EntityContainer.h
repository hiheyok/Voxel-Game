#pragma once
#include <mutex>

#include "../Typenames.h"

class Dimension;

struct Entity;
struct EntityProperty;

class EntityContainer { //Manages all entities in world
public:
    void AddEntities(Entity& e);

    void AddEntities(Entity* e);

    int GetEntityCount() const;

    FastHashMap<EntityUUID, EntityProperty> ClientGetEntityUpdate();

    void RemoveEntity(EntityUUID entityId);

    FastHashSet<EntityUUID> getRemovedEntities();

    Entity* GetEntity(EntityUUID entityId);

    void Tick(Dimension* dimension);
private:
    FastHashMap<EntityUUID, Entity*> entities_;
    FastHashSet<EntityUUID> removed_entity_;
    std::mutex entity_lock_;
    EntityUUID unique_id_ = 0;
    int entity_count_ = 0;
};
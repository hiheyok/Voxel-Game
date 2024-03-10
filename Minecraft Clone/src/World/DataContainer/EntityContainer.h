#pragma once
#include "../Entity/Entity.h"
#include <unordered_map>

class EntityContainer { //Manages all entities in world
private:
	std::unordered_map<EntityUUID, Entity> Entities;
	std::unordered_set<EntityUUID> RemovedEntity;
	EntityUUID UniqueID = 0;
	int EntityCount = 0;
public:
	void AddEntities(Entity& e) {
		e.Properties.EntityUUID = UniqueID;
		Entities.emplace(UniqueID, e);
		UniqueID++;
		EntityCount++;
	}

	int GetEntityCount() {
		return EntityCount;
	}

	std::unordered_map<EntityUUID, EntityProperty> ClientGetEntityUpdate() {
		std::unordered_map<EntityUUID, EntityProperty> UpdatedData;

		for (auto& entity : Entities) {
			if (entity.second.isDirty)  {
				entity.second.isDirty = false;
				UpdatedData.emplace(entity.second.Properties.EntityUUID, entity.second.Properties);
			}
		}

		return UpdatedData;
	}

	void RemoveEntity(EntityUUID EntityID) {
		Entities.erase(EntityID);
		RemovedEntity.emplace(EntityID);
		EntityCount--;
	}

	std::unordered_set<EntityUUID> getRemovedEntities() {
		std::unordered_set<EntityUUID> a = RemovedEntity;
		RemovedEntity.clear();
		return a;
	}



	Entity* getEntity(EntityUUID EntityID) {
		if (Entities.count(EntityID))
			return &Entities[EntityID];
		return nullptr;
	}

	void Tick() {
		for (auto& entity : Entities) {
			entity.second.Tick();
		}
	}

};
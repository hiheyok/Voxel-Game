#pragma once
#include "../Entity/Entity.h"
#include <unordered_map>
#include <mutex>

class EntityContainer { //Manages all entities in world
private:
	std::unordered_map<EntityUUID, Entity*> Entities;
	std::unordered_set<EntityUUID> RemovedEntity;
	std::mutex EntityLock;
	EntityUUID UniqueID = 0;
	int EntityCount = 0;
public:
	void AddEntities(Entity& e) {
		e.Properties.EntityUUID = UniqueID;
		Entities.emplace(UniqueID, &e);
		UniqueID++;
		EntityCount++;
	}

	void AddEntities(Entity* e) {
		e->Properties.EntityUUID = UniqueID;
		Entities.emplace(UniqueID, e);
		UniqueID++;
		EntityCount++;
	}

	int GetEntityCount() {
		return EntityCount;
	}

	std::unordered_map<EntityUUID, EntityProperty> ClientGetEntityUpdate() { //change this to past on a vector later
		std::unordered_map<EntityUUID, EntityProperty> UpdatedData;

		for (auto& entity : Entities) {
			if (entity.second->isDirty)  {
				entity.second->isDirty = false;
				UpdatedData.emplace(entity.second->Properties.EntityUUID, entity.second->Properties);
			}
		}

		return UpdatedData;
	}

	void RemoveEntity(EntityUUID EntityID) {
		delete Entities[EntityID];
		Entities.erase(EntityID);
		EntityLock.lock();
		RemovedEntity.emplace(EntityID);
		EntityLock.unlock();
		EntityCount--;
	}

	std::unordered_set<EntityUUID> getRemovedEntities() {
		std::unordered_set<EntityUUID> a;
		EntityLock.lock();
		a = RemovedEntity;
		RemovedEntity.clear();
		EntityLock.unlock();
		return a;
	}



	Entity* getEntity(EntityUUID EntityID) {
		if (Entities.count(EntityID))
			return Entities[EntityID];
		return nullptr;
	}

	void Tick() {
		for (auto& entity : Entities) {
			entity.second->Tick();
		}
	}

};
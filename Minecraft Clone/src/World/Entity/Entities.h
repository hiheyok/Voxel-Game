#pragma once

#include "Type/EntityType.h"

#include <unordered_map>
#include <deque>


enum EntityTypeEnums {
	ENTITY_PASSIVE, ENTITY_HOSTILE, ENTITY_FALLING_BLOCK
};

struct EntityRegistration {
	EntityTypeEnums Type;
	std::string EntityName;
	EntityTypeID id = 0;
};

class EntitiesList {
private:
	std::deque<EntityRegistration> RegisterQueue;

	int EntityTypeCount = 0;

	void InitializeModels();
	std::unordered_map<std::string, EntityTypeID> EntityNameID;
public:
	std::unordered_map<EntityTypeID, EntityType*> EntityTypeList;

	EntityTypeID HUMAN = QueueRegisterEntity("human", ENTITY_PASSIVE);
	EntityTypeID PLAYER = QueueRegisterEntity("player", ENTITY_PASSIVE);
	EntityTypeID ZOMBIE = QueueRegisterEntity("zombie", ENTITY_HOSTILE);
	EntityTypeID SAND_GRAVITY_BLOCK = QueueRegisterEntity("sand", ENTITY_FALLING_BLOCK);

	EntitiesList() {

	}

	void RegisterNewEntity(EntityRegistration reg);

	void RegisterAll();

	void Initialize() {
		RegisterAll();
		InitializeModels();
	}

	

	inline EntityTypeID QueueRegisterEntity(std::string EntityName, EntityTypeEnums type) {
		EntityRegistration reg;
		reg.Type = type;
		reg.id = EntityTypeCount;
		reg.EntityName = EntityName;
		EntityTypeCount++;

		RegisterQueue.push_back(reg);

		return EntityTypeCount - 1;
	}

	EntityType* GetEntity(EntityTypeID id) {
		return EntityTypeList[id];
	}

} __declspec(selectany) EntityList;



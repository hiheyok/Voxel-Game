#pragma once

#include "Type/EntityType.h"

#include <unordered_map>
#include <deque>


enum EntityTypeEnums {
	ENTITY_PASSIVE, ENTITY_HOSTILE, ENTITY_FALLING_BLOCK
};

class EntitiesList {
private:
	int EntityTypeCount = 0;

	std::unordered_map<std::string, EntityTypeID> EntityNameID;
public:
	std::unordered_map<EntityTypeID, EntityType*> EntityTypeList;

	EntityTypeID HUMAN = RegisterEntity("human", ENTITY_PASSIVE);
	EntityTypeID PLAYER = RegisterEntity("player", ENTITY_PASSIVE);
	EntityTypeID ZOMBIE = RegisterEntity("zombie", ENTITY_HOSTILE);
	EntityTypeID SAND_GRAVITY_BLOCK = RegisterEntity("sand", ENTITY_FALLING_BLOCK);

	EntitiesList() {

	}

	void InitializeModels();

	EntityTypeID RegisterEntity(std::string EntityName, EntityTypeEnums type);

	void Initialize() {
		InitializeModels();
	}

	EntityType* GetEntity(EntityTypeID id) {
		return EntityTypeList[id];
	}

};

extern EntitiesList EntityList;



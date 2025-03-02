#pragma once

#include "Type/EntityType.h"

#include <deque>


enum EntityTypeEnums {
	ENTITY_PASSIVE, ENTITY_HOSTILE, ENTITY_FALLING_BLOCK
};

class EntitiesList {
private:
	int entity_type_count_ = 0;

	FastHashMap<std::string, EntityTypeID> EntityNameID;
public:
	std::vector<EntityType*> entity_type_list_;

	EntityTypeID HUMAN = RegisterEntity("human", ENTITY_PASSIVE);
	EntityTypeID PLAYER = RegisterEntity("player", ENTITY_PASSIVE);
	EntityTypeID ZOMBIE = RegisterEntity("zombie", ENTITY_HOSTILE);
	EntityTypeID SAND_GRAVITY_BLOCK = RegisterEntity("sand", ENTITY_FALLING_BLOCK);

	EntitiesList() {

	}

	void InitializeModels();

	EntityTypeID RegisterEntity(std::string entityName, EntityTypeEnums type_);

	void Initialize();

	EntityType* GetEntity(EntityTypeID id);

};

extern EntitiesList g_entity_list;



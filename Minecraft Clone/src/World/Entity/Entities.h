#pragma once

#include "EntityType.h"
#include <unordered_map>

__declspec(selectany) std::unordered_map<EntityID, EntityType*> EntityTypeList;



inline EntityID RegisterNewEntity() {
	EntityID ID = (EntityID)EntityTypeList.size();
	EntityType* NewEntity = new EntityType();

	NewEntity->ID = ID;

	EntityTypeList[ID] = NewEntity;

	return ID;
}

__declspec(selectany) EntityID PLAYER = RegisterNewEntity();

inline void InitializeEntities() {
	EntityTypeList[PLAYER]->ChangeHitboxSize(0.8,1.8,0.8);
}
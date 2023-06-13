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
__declspec(selectany) EntityID HUMAN = RegisterNewEntity();

inline void InitializeEntities() {
	EntityTypeList[PLAYER]->ChangeHitboxSize(0.8f, 1.8f, 0.8f);
	EntityTypeList[PLAYER]->RenderModel.AddRectangle(0.8f, 1.8f, 0.8f);

	EntityTypeList[HUMAN]->ChangeHitboxSize(0.8f, 1.8f, 0.8f);
	EntityTypeList[HUMAN]->RenderModel.AddRectangle(0.8f, 1.8f, 0.8f);
}
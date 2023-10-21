#pragma once

#include "EntityType.h"
#include <unordered_map>
#include <iostream>

class EntityTypes {
private:
	std::unordered_map<EntityID, EntityType*> EntityTypeList;
public:

	EntityID HUMAN = RegisterNewEntity();
	EntityID PLAYER = RegisterNewEntity();

	EntityTypes() {

		EntityTypeList[PLAYER]->ChangeHitboxSize(0.8f, 1.8f, 0.8f);
		EntityTypeList[PLAYER]->RenderModel.AddRectangle(0.8f, 1.8f, 0.8f);

		EntityTypeList[HUMAN]->ChangeHitboxSize(0.8f, 1.8f, 0.8f);
		EntityTypeList[HUMAN]->RenderModel.AddRectangle(0.8f, 1.8f, 0.8f);
	}

	inline EntityID RegisterNewEntity() {
		EntityID ID = (EntityID)EntityTypeList.size();
		EntityType* NewEntity = new EntityType();

		NewEntity->ID = ID;

		EntityTypeList[ID] = NewEntity;

		return ID;
	}

	EntityType* GetEntity(EntityID id) {
		return EntityTypeList[id];
	}

} __declspec(selectany) EntityList;



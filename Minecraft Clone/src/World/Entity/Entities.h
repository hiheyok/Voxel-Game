#pragma once

#include "EntityType.h"
#include <unordered_map>
#include <iostream>

class EntityTypes {
private:
	
public:
	std::unordered_map<EntityID, EntityType*> EntityTypeList;

	EntityID HUMAN = RegisterNewEntity();
	EntityID PLAYER = RegisterNewEntity();
	EntityID ZOMBIE = RegisterNewEntity();

	EntityTypes() {

		EntityTypeList[PLAYER]->ChangeHitboxSize(0.8f, 1.8f, 0.8f);
		EntityTypeList[PLAYER]->RenderModel.AddRectangle(0.8f, 1.8f, 0.8f);

		EntityTypeList[HUMAN]->ChangeHitboxSize(0.8f, 1.8f, 0.8f);
		EntityTypeList[HUMAN]->RenderModel.AddRectangle(0.8f, 1.8f, 0.8f);

		EntityTypeList[ZOMBIE]->ChangeHitboxSize(0.8f, 1.8f, 0.8f);
		EntityTypeList[ZOMBIE]->RenderModel.AddRectangle(0.8f, 1.8f, 0.8f);
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



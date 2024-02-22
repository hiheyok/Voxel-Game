#pragma once

#include "Type/EntityType.h"
#include <unordered_map>
#include <iostream>

class EntityTypes {
private:
	
public:
	std::unordered_map<EntityTypeID, EntityType*> EntityTypeList;

	EntityTypeID HUMAN = RegisterNewEntity();
	EntityTypeID PLAYER = RegisterNewEntity();
	EntityTypeID ZOMBIE = RegisterNewEntity();

	EntityTypes() {

	}

	void Initialize() {
		EntityTypeList[PLAYER]->ChangeHitboxSize(0.8f, 1.8f, 0.8f);
		EntityTypeList[PLAYER]->RenderModel.AddRectangle(glm::vec3(0.8f, 1.8f, 0.8f), glm::vec3(0.f, 0.f, 0.f));

		EntityTypeList[HUMAN]->ChangeHitboxSize(0.8f, 1.8f, 0.8f);
		EntityTypeList[HUMAN]->RenderModel.AddRectangle(glm::vec3(0.8f, 1.8f, 0.8f), glm::vec3(0.f, 0.f, 0.f));

		EntityTypeList[ZOMBIE]->ChangeHitboxSize(0.8f, 2.0f, 0.8f);
		EntityTypeList[ZOMBIE]->RenderModel.AddRectangle(glm::vec3(0.25f, 0.75f, 0.25f), glm::vec3(0.f, 0.f, 0.f)); //legs
		EntityTypeList[ZOMBIE]->RenderModel.AddRectangle(glm::vec3(0.25f, 0.75f, 0.25f), glm::vec3(0.25f, 0.f, 0.f)); //legs
		EntityTypeList[ZOMBIE]->RenderModel.AddRectangle(glm::vec3(0.50f, 0.75f, 0.25f), glm::vec3(0.f, 0.75f, 0.f)); //TORSO
		EntityTypeList[ZOMBIE]->RenderModel.AddRectangle(glm::vec3(0.50f, 0.50f, 0.50f), glm::vec3(0.f, 1.5f, -0.125f)); //HEAD
		EntityTypeList[ZOMBIE]->RenderModel.AddRectangle(glm::vec3(0.50f, 0.50f, 0.50f), glm::vec3(0.f, 1.5f, -0.125f)); //HEAD
		EntityTypeList[ZOMBIE]->RenderModel.AddRectangle(glm::vec3(0.25f, 0.75f, 0.25f), glm::vec3(-0.25f, 0.75f, 0.f)); //ARM
		EntityTypeList[ZOMBIE]->RenderModel.AddRectangle(glm::vec3(0.25f, 0.75f, 0.25f), glm::vec3(0.50f, 0.75f, 0.f)); //ARM
	}

	inline EntityTypeID RegisterNewEntity() {
		EntityTypeID ID = (EntityTypeID)EntityTypeList.size();
		EntityType* NewEntity = new EntityType();

		NewEntity->ID = ID;

		EntityTypeList[ID] = NewEntity;

		return ID;
	}

	EntityType* GetEntity(EntityTypeID id) {
		return EntityTypeList[id];
	}

} __declspec(selectany) EntityList;



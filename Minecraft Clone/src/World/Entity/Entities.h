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
public:
	std::unordered_map<EntityTypeID, EntityType*> EntityTypeList;

	EntityTypeID HUMAN = QueueRegisterEntity("Human", ENTITY_PASSIVE);
	EntityTypeID PLAYER = QueueRegisterEntity("Player", ENTITY_PASSIVE);
	EntityTypeID ZOMBIE = QueueRegisterEntity("zombie", ENTITY_HOSTILE);
	EntityTypeID SAND_GRAVITY_BLOCK = QueueRegisterEntity("sand", ENTITY_FALLING_BLOCK);

	EntitiesList() {

	}

	void RegisterNewEntity(EntityRegistration reg);

	void RegisterAll();

	void Initialize() {
		RegisterAll();

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

		EntityTypeList[SAND_GRAVITY_BLOCK]->ChangeHitboxSize(1.f, 1.f, 1.f);
		EntityTypeList[SAND_GRAVITY_BLOCK]->RenderModel.AddRectangle(glm::vec3(1.f,1.f,1.f),glm::vec3(0.f,0.f,0.f));
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



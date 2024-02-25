#include "Entities.h"

#include "Type/Passive.h"
#include "Type/FallingBlock.h"
#include "Type/Hostile.h"

void EntitiesList::RegisterNewEntity(EntityRegistration reg) {
	EntityTypeID ID = reg.id;
	EntityType* NewEntity;

	switch (reg.Type) {
	case ENTITY_PASSIVE:
		NewEntity = static_cast<EntityType*>(new Passive());
		break;
	case ENTITY_FALLING_BLOCK:
		NewEntity = static_cast<EntityType*>(new FallingBlock());
		break;
	case ENTITY_HOSTILE:
		NewEntity = static_cast<EntityType*>(new Hostile());
		break;
	}

	NewEntity->EntityName = reg.EntityName;

	Logger.LogInfo("Entity Register", "Registered new entity: " + reg.EntityName + " | EntityID: " + std::to_string(reg.id));

	NewEntity->ID = ID;

	EntityTypeList[ID] = NewEntity;

}

void EntitiesList::RegisterAll() {
	for (auto& reg : RegisterQueue) {
		RegisterNewEntity(reg);
	}

	RegisterQueue.clear();
}
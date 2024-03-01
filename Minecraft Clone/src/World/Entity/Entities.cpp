#include "Entities.h"

#include "Type/Passive.h"
#include "Type/FallingBlock.h"
#include "Type/Hostile.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using namespace std;
using namespace glm;

using json = nlohmann::json;

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
    EntityNameID[reg.EntityName] = ID;

}

void EntitiesList::RegisterAll() {
	for (auto& reg : RegisterQueue) {
		RegisterNewEntity(reg);
	}

	RegisterQueue.clear();
}

void EntitiesList::InitializeModels() {
    ifstream file("assets/EntityShape.json");
	
    json data = json::parse(file);

    for (auto& b : data.items()) {
        EntityTypeID entityType = EntityNameID[b.key()];

        json::iterator d = b.value().begin();

        vec3 hitboxSize(d.value().at(0), d.value().at(1), d.value().at(2));

        EntityTypeList[entityType]->ChangeHitboxSize(hitboxSize);
		
        d++;

        for (auto& SubData : d.value().items()) {
            json::iterator it = SubData.value().begin();

            vec3 Offset(it.value().at(0), it.value().at(1), it.value().at(2));

            it++;

            vec3 ShapeSize(it.value().at(0), it.value().at(1), it.value().at(2));

			EntityTypeList[entityType]->RenderModel.AddRectangle(ShapeSize, Offset);
        }
    }
}
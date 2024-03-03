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

		if (d.value().is_string()) {
			Logger.LogInfo("Entity Texture", "Entity: " + b.key() + " | Texture Loading: " + (string)d.value());
			RawTextureData TexData;
			TexData.Load(((string)d.value()).c_str());
			EntityTypeList[entityType]->texture.Gen();
			EntityTypeList[entityType]->texture.Load(TexData);
		}

		d++;

        vec3 hitboxSize(d.value().at(0), d.value().at(1), d.value().at(2));

        EntityTypeList[entityType]->ChangeHitboxSize(hitboxSize);
		
        d++; 

        for (auto& SubData : d.value().items()) {
            json::iterator it = SubData.value().begin();

            vec3 Offset(it.value().at(0), it.value().at(1), it.value().at(2));

            it++;

            vec3 ShapeSize(it.value().at(0), it.value().at(1), it.value().at(2));

			it++;

			Model::RectangularPrism* model = EntityTypeList[entityType]->RenderModel.AddRectangle(ShapeSize, Offset);

			for (auto& ShapeUV : it.value().items()) {
				json::iterator uv_iterator = ShapeUV.value().begin();

				vector<int> UV_Faces = {};

				for (auto& UV_Face : uv_iterator.value().items()) {
					int s = 0xFF;

					string texSide = UV_Face.value();

					if (!strcmp(texSide.c_str(), "FRONT")) {
						s = FRONT;
					}
					if (!strcmp(texSide.c_str(), "BACK")) {
						s = BACK;
					}
					if (!strcmp(texSide.c_str(), "LEFT")) {
						s = LEFT;
					}
					if (!strcmp(texSide.c_str(), "RIGHT")) {
						s = RIGHT;
					}
					if (!strcmp(texSide.c_str(), "TOP")) {
						s = TOP;
					}
					if (!strcmp(texSide.c_str(), "BOTTOM")) {
						s = BOTTOM;
					}

					UV_Faces.push_back(s);
				}

				uv_iterator++;
				
				vec2 pts[2]{};

				int index = 0;

				for (auto& UV_Points : uv_iterator.value().items()) { //iterate though uv points
					pts[index] = vec2(UV_Points.value().at(0), UV_Points.value().at(1));
					index++;
				}

				for (int& face : UV_Faces) {
					model->UV_MAP[face].p0 = pts[0];
					model->UV_MAP[face].p1 = pts[1];
				}
			}
        }
    }
}
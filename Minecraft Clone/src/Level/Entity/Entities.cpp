#include "Entities.h"

#include "Type/Types.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using namespace std;
using namespace glm;

using json = nlohmann::json;

EntityTypeID EntitiesList::RegisterEntity(std::string EntityName, EntityTypeEnums type_) {
	EntityTypeID ID = static_cast<EntityTypeID>(entity_type_list_.size());
	EntityType* newEntity = nullptr;

	switch (type_) {
	case ENTITY_PASSIVE:
		newEntity = static_cast<EntityType*>(new Passive());
		break;
	case ENTITY_FALLING_BLOCK:
		newEntity = static_cast<EntityType*>(new FallingBlock());
		break;
	case ENTITY_HOSTILE:
		newEntity = static_cast<EntityType*>(new Hostile());
		break;
	}

	if (newEntity == nullptr) {
		throw std::exception("Error!");
	}

	newEntity->entity_name_ = EntityName;

	g_logger.LogInfo("Entity Register", "Registered new entity: " + EntityName + " | EntityID: " + std::to_string(ID));

	newEntity->id_ = ID;

	entity_type_list_.emplace_back(newEntity);
    EntityNameID[EntityName] = ID;

	return ID;

}


void EntitiesList::InitializeModels() {
    ifstream file("assets/EntityShape.json");
	
    json data = json::parse(file);

    for (auto& b : data.items()) {
        EntityTypeID entityType = EntityNameID[b.key()];

        json::iterator d = b.value().begin();

		if (d.value().is_string()) {
			g_logger.LogInfo("Entity Texture", "Entity: " + b.key() + " | Texture Loading: " + (string)d.value());
			RawTextureData TexData;
			TexData.Load(((string)d.value()).c_str());
			entity_type_list_[entityType]->texture_.Gen();
			entity_type_list_[entityType]->texture_.Load(TexData);
		}

		d++;

        vec3 hitboxSize(d.value().at(0), d.value().at(1), d.value().at(2));

        entity_type_list_[entityType]->ChangeHitboxSize(hitboxSize);
		
        d++; 

        for (auto& SubData : d.value().items()) {
            json::iterator it = SubData.value().begin();

            vec3 offset(it.value().at(0), it.value().at(1), it.value().at(2));

            it++;

            vec3 shapeSize(it.value().at(0), it.value().at(1), it.value().at(2));

			it++;

			Model::RectangularPrism* model = entity_type_list_[entityType]->render_model_.AddRectangle(shapeSize, offset);

			for (auto& ShapeUV : it.value().items()) {
				json::iterator uv_iterator = ShapeUV.value().begin();

				vector<int> uvFaces = {};

				for (auto& uvFace : uv_iterator.value().items()) {
					int s = 0xFF;

					string texSide = uvFace.value();

					if (!strcmp(texSide.c_str(), "FRONT"))
						s = FRONT;
					if (!strcmp(texSide.c_str(), "BACK"))
						s = BACK;
					if (!strcmp(texSide.c_str(), "LEFT"))
						s = LEFT;
					if (!strcmp(texSide.c_str(), "RIGHT"))
						s = RIGHT;
					if (!strcmp(texSide.c_str(), "TOP"))
						s = TOP;
					if (!strcmp(texSide.c_str(), "BOTTOM"))
						s = BOTTOM;

					uvFaces.push_back(s);
				}

				uv_iterator++;
				
				vec2 pts[2]{};

				int index = 0;

				for (auto& UV_Points : uv_iterator.value().items()) { //iterate though uv points
					pts[1 - index] = vec2((float)UV_Points.value().at(0), 1.f - (float)UV_Points.value().at(1));
					index++;
				}

				for (int& face : uvFaces) {
					model->uv_map_[face].p0_ = pts[0];
					model->uv_map_[face].p1_ = pts[1];
				}
			}
        }
    }
}

void EntitiesList::Initialize() {
	InitializeModels();
}

EntityType* EntitiesList::GetEntity(EntityTypeID id) {
	return entity_type_list_[id];
}
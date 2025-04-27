#include <fstream>
#include <nlohmann/json.hpp>

#include "Level/Entity/Entities.h"
#include "Level/Entity/Type/Types.h"

using json = nlohmann::json;

EntitiesList::EntitiesList() {

}

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
        g_logger.LogError("RegisterEntity", "Error!");
    }

    newEntity->entity_name_ = EntityName;

    g_logger.LogInfo("EntitiesList::RegisterEntity", "Registered new entity: " + EntityName + " | EntityID: " + std::to_string(ID));

    newEntity->id_ = ID;

    entity_type_list_.emplace_back(newEntity);
    entity_name_id_[EntityName] = ID;

    return ID;

}


void EntitiesList::InitializeModels() {
    std::ifstream file("assets/EntityShape.json");
    
    json data = json::parse(file);

    for (auto& b : data.items()) {
        EntityTypeID entityType = entity_name_id_[b.key()];

        json::iterator d = b.value().begin();

        if (d.value().is_string()) {
            g_logger.LogInfo("EntitiesList::InitializeModels", "Entity: " + b.key() + " | Texture Loading: " + (std::string)d.value());
            RawTextureData TexData{ d.value() };
            entity_type_list_[entityType]->texture_ = std::make_unique<Texture2D>(TexData);
        }

        d++;

        glm::vec3 hitboxSize(d.value().at(0), d.value().at(1), d.value().at(2));

        entity_type_list_[entityType]->ChangeHitboxSize(hitboxSize);
        
        d++; 

        for (auto& SubData : d.value().items()) {
            json::iterator it = SubData.value().begin();

            glm::vec3 offset(it.value().at(0), it.value().at(1), it.value().at(2));

            it++;

            glm::vec3 shapeSize(it.value().at(0), it.value().at(1), it.value().at(2));

            it++;

            Model::RectangularPrism* model = entity_type_list_[entityType]->render_model_.AddRectangle(shapeSize, offset);

            for (auto& ShapeUV : it.value().items()) {
                json::iterator uv_iterator = ShapeUV.value().begin();

                std::vector<int> uvFaces = {};

                for (auto& uvFace : uv_iterator.value().items()) {
                    int s = 0xFF;

                    std::string texSide = uvFace.value();

                    // TODO: refactor FRONT BACK LEFT RIGHT TOP BOTTOM to new directions
                    if (texSide == "FRONT")
                        s = Directions<BlockPos>::kEast;
                    if (texSide == "BACK")
                        s = Directions<BlockPos>::kWest;
                    if (texSide == "LEFT")
                        s = Directions<BlockPos>::kNorth;
                    if (texSide == "RIGHT")
                        s = Directions<BlockPos>::kSouth;
                    if (texSide == "TOP")
                        s = Directions<BlockPos>::kUp;
                    if (texSide == "BOTTOM")
                        s = Directions<BlockPos>::kDown;

                    uvFaces.push_back(s);
                }

                uv_iterator++;
                
                glm::vec2 pts[2]{};

                int index = 0;

                for (auto& UV_Points : uv_iterator.value().items()) { //iterate though uv points
                    pts[1 - index] = glm::vec2((float)UV_Points.value().at(0), 1.f - (float)UV_Points.value().at(1));
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
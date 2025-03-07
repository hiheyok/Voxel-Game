#pragma once
#include "../../Typenames.h"
#include "../Collusion/Hitbox.h"
#include "../../../RenderEngine/EntityRenderer/Models/EntityModel.h"
#include "../Entity.h"
#include "../../../RenderEngine/OpenGL/Texture/Types/Texture2D.h"

class EntityType {
public:
    
    EntityType() {

    }
    
    void ChangeHitboxSize(glm::vec3 size) {
        hitbox_.size_ = size;
    }

    void ChangeHitboxSize(float x, float y, float z) {
        hitbox_.size_ = glm::vec3(x, y, z);
    }

    AABB& GetHitbox() {
        return hitbox_;
    }

    virtual void Tick(Entity* entity) {
        (void)entity;
    }

    EntityTypeID id_ = NULL;

    EntityModel render_model_;

    Texture2D texture_;

    std::string entity_name_;

private:
    
    AABB hitbox_;
};



#pragma once
#include "Core/Typenames.h"
#include "Level/Entity/Collusion/Hitbox.h"
#include "Level/Entity/Entity.h"
#include "RenderEngine/EntityRender/Models/EntityModel.h"
#include "RenderEngine/OpenGL/Texture/Types/Texture2D.h"

class Dimension;

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

    virtual void Tick(Entity* entity, Dimension* dimension) = 0;

    EntityTypeID id_ = 0;

    EntityModel render_model_;

    std::unique_ptr<Texture2D> texture_;

    std::string entity_name_;

private:
    
    AABB hitbox_;
};



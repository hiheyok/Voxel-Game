#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "../../Typenames.h"

struct EntityProperty {
    glm::vec3 velocity_ = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 position_ = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 acceleration_ = glm::vec3(0.f, 0.f, 0.f);
    glm::vec2 rotation_ = glm::vec2(0.f, 0.f);

    float max_speed_ = 50;

    bool on_ground_ = false;

    bool is_chunk_loader_ = false;

    EntityUUID entity_uuid_ = 0x00000000;
    EntityTypeID type_ = NULL; //State what type of entity is it. Zombie? Human??
};
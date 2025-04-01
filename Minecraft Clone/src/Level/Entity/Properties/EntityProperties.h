#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "../../Typenames.h"

struct EntityProperty {
    EntityProperty() = default;
    EntityProperty(const EntityProperty&) = default;
    ~EntityProperty() = default;

    glm::vec3 velocity_ = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 position_ = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 acceleration_ = glm::vec3(0.f, 0.f, 0.f);
    glm::vec2 rotation_ = glm::vec2(0.f, 0.f);

    float max_speed_ = 50;

    bool on_ground_ = false;

    bool is_chunk_loader_ = false;

    EntityUUID entity_uuid_ = 0;
    EntityTypeID type_ = 0; //State what type of entity is it. Zombie? Human??

    bool operator==(const EntityProperty& other) {
        return entity_uuid_ == other.entity_uuid_;
    }
};

namespace std {
    template <>
    struct hash<EntityProperty> {
        size_t operator ()(const EntityProperty& e) const {
            return hash<EntityTypeID>{}(e.entity_uuid_);
        }
    };
    
}
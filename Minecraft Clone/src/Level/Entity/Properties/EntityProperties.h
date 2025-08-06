// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Core/Typenames.h"

struct EntityProperty {
  EntityProperty() = default;
  EntityProperty(const EntityProperty&) = default;
  ~EntityProperty() = default;

  glm::vec3 velocity_ = {0.0f, 0.0f, 0.0f};
  glm::vec3 position_ = {0.0f, 0.0f, 0.0f};
  glm::vec3 acceleration_ = {0.0f, 0.0f, 0.0f};
  glm::vec2 rotation_ = {0.0f, 0.0f};

  float max_speed_ = 50;

  bool on_ground_ = false;

  bool is_chunk_loader_ = false;

  EntityUUID entity_uuid_ = 0;
  EntityTypeID type_ = 0;  // State what type of entity is it. Zombie? Human??

  bool operator==(const EntityProperty& other) {
    return entity_uuid_ == other.entity_uuid_;
  }
};

namespace std {
template <>
struct hash<EntityProperty> {
  size_t operator()(const EntityProperty& e) const {
    return hash<EntityUUID>{}(e.entity_uuid_);
  }
};

}  // namespace std

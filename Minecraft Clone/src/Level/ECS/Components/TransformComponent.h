#pragma once

#include <glm/vec3.hpp>

struct TransformComponent {
  TransformComponent();

  // The entity location
  glm::vec3 position_;
  // The entity velocity
  glm::vec3 velocity_;
  // The entity acceleration
  glm::vec3 acceleration_;
};

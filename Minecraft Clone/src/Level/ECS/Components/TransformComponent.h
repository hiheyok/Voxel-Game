#pragma once

#include <glm/vec3.hpp>


class TransformComponent {
  public:
    TransformComponent();

    // The entity location
    glm::vec3 position_;
    // The entity velocity
    glm::vec3 velocity_;
    // The entity acceleration
    glm::vec3 acceleration_;
};

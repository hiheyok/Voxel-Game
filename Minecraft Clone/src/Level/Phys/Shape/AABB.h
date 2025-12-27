#pragma once

#include <glm/vec3.hpp>

struct AABB {
  AABB();
  AABB(glm::vec3 min, glm::vec3 max);

  AABB Translate(glm::vec3 offset) const noexcept;
  bool Intersects(const AABB& other) const noexcept;
  bool Contains(glm::vec3 point) const noexcept;

  void Encapsulate(const AABB& other) noexcept;
  
  glm::vec3 min_;
  glm::vec3 max_;
};

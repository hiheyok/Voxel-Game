#include "Level/Phys/Shape/AABB.h"

#include <glm/common.hpp>
#include <glm/vec3.hpp>

using glm::vec3;

AABB::AABB() : min_{0.0f}, max_{0.0f} {}
AABB::AABB(vec3 min, vec3 max) : min_{min}, max_{max} {}

AABB AABB::Translate(glm::vec3 offset) const noexcept {
  return AABB{min_ + offset, max_ + offset};
}

bool AABB::Intersects(const AABB& other) const noexcept {
  return (min_.x <= other.max_.x && max_.x >= other.min_.x) &&
         (min_.y <= other.max_.y && max_.y >= other.min_.y) &&
         (min_.z <= other.max_.z && max_.z >= other.min_.z);
}

bool AABB::Contains(glm::vec3 point) const noexcept {
  return (point.x >= min_.x && point.x <= max_.x) &&
         (point.y >= min_.y && point.y <= max_.y) &&
         (point.z >= min_.z && point.z <= max_.z);
}

void AABB::Encapsulate(const AABB& other) noexcept {
  min_ = glm::min(min_, other.min_);
  max_ = glm::max(max_, other.max_);
}

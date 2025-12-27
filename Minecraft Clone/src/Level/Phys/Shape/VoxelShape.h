#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "Level/Phys/Shape/AABB.h"

class VoxelShape {
 public:
  void AddBox(const AABB& cuboid);
  void Clear();

  const std::vector<AABB>& GetBoxes() const;
  const AABB& GetRootBounds() const;

 private:
  std::vector<AABB> boxes_;
  AABB root_bounds_;
};

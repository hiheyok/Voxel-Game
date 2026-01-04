#include "Level/Phys/Shape/VoxelShape.h"

#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "Level/Phys/Shape/AABB.h"

using glm::vec3;
using std::vector;

void VoxelShape::AddBox(const AABB& box) {
  if (boxes_.empty()) {
    root_bounds_ = box;
  } else {
    root_bounds_.min_ = glm::min(root_bounds_.min_, box.min_);
    root_bounds_.max_ = glm::max(root_bounds_.max_, box.max_);
  }

  boxes_.push_back(box);
}

void VoxelShape::Clear() {
  boxes_.clear();
  root_bounds_.min_ = {0.0f, 0.0f, 0.0f};
  root_bounds_.max_ = {0.0f, 0.0f, 0.0f};
}

const vector<AABB>& VoxelShape::GetBoxes() const { return boxes_; }

const AABB& VoxelShape::GetRootBounds() const { return root_bounds_; }

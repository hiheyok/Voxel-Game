#include "Level/Phys/VoxelCollider.h"

#include <algorithm>
#include <glm/glm.hpp>

#include "Level/Phys/Shape/VoxelShape.h"

VoxelCollider::CollisionResult VoxelCollider::CheckCollision(
    const VoxelShape& shape_a, const glm::vec3& pos_a,
    const VoxelShape& shape_b, const glm::vec3& pos_b) {
  CollisionResult result;

  AABB root_a = shape_a.GetRootBounds().Translate(pos_a);
  AABB root_b = shape_b.GetRootBounds().Translate(pos_b);

  // Fast check
  if (!root_a.Intersects(root_b)) {
    return result;
  }

  // Secondary check
  for (const auto& box_a : shape_a.GetBoxes()) {
    AABB world_a = box_a.Translate(pos_a);

    for (const auto& box_b : shape_b.GetBoxes()) {
      AABB world_b = box_b.Translate(pos_b);

      if (world_a.Intersects(world_b)) {
        result.is_hit = true;
        return result;  // hit
      }
    }
  }
}

VoxelCollider::CollisionSweptResult VoxelCollider::CheckSweepCollision(
    const VoxelShape& moving_shape, const glm::vec3& moving_pos,
    const glm::vec3& moving_vel, const VoxelShape& static_shape,
    const glm::vec3& static_pos) {
  CollisionSweptResult best_result;
  best_result.time = 1.0f;  // Default: Completed the whole move without hitting
  best_result.is_hit = false;

  // If velocity is negligible, skip the swept check
  if (glm::length(moving_vel) < 0.0001f) {
    return best_result;
  }

  // Approximately test if the object intersects
  AABB move_root_start = moving_shape.GetRootBounds().Translate(moving_pos);
  AABB static_root = static_shape.GetRootBounds().Translate(static_pos);

  AABB swept_broad_phase = move_root_start;
  swept_broad_phase.Encapsulate(move_root_start.Translate(moving_vel));

  if (!swept_broad_phase.Intersects(static_root)) {
    return best_result;
  }

  for (const auto& box_m : moving_shape.GetBoxes()) {
    AABB world_m = box_m.Translate(moving_pos);

    for (const auto& box_s : static_shape.GetBoxes()) {
      AABB world_s = box_s.Translate(static_pos);

      CollisionSweptResult temp_result =
          SweepAABB(world_m, moving_vel, world_s);

      if (temp_result.is_hit && temp_result.time < best_result.time) {
        best_result = temp_result;
      }
    }
  }

  return best_result;
}

VoxelCollider::CollisionSweptResult VoxelCollider::SweepAABB(
    const AABB& moving_box, const glm::vec3& velocity, const AABB& static_box) {
  CollisionSweptResult result;
  result.time = 1.0f;

  glm::vec3 inv_entry, inv_exit;
  glm::vec3 entry_time, exit_time;

  for (int i = 0; i < 3; i++) {
    if (velocity[i] > 0.0f) {
      inv_entry[i] = static_box.min_[i] - moving_box.max_[i];
      inv_exit[i] = static_box.max_[i] - moving_box.min_[i];
    } else {
      inv_entry[i] = static_box.max_[i] - moving_box.min_[i];
      inv_exit[i] = static_box.min_[i] - moving_box.max_[i];
    }

    if (velocity[i] == 0.0f) {
      if (moving_box.max_[i] <= static_box.min_[i] ||
          moving_box.min_[i] >= static_box.max_[i]) {
        return result;
      }
      entry_time[i] = -std::numeric_limits<float>::infinity();
      exit_time[i] = std::numeric_limits<float>::infinity();
    } else {
      entry_time[i] = inv_entry[i] / velocity[i];
      exit_time[i] = inv_exit[i] / velocity[i];
    }
  }

  float entry_time_max = std::max({entry_time.x, entry_time.y, entry_time.z});
  float exit_time_min = std::min({exit_time.x, exit_time.y, exit_time.z});

  if (entry_time_max > exit_time_min || entry_time_max > 1.0f ||
      entry_time_max < 0.0f) {
    return result;
  }

  result.is_hit = true;
  result.time = entry_time_max;

  if (entry_time.x > entry_time.y && entry_time.x > entry_time.z) {
    result.normal =
        (velocity.x < 0.0f) ? glm::vec3(1, 0, 0) : glm::vec3(-1, 0, 0);
  } else if (entry_time.y > entry_time.z) {
    result.normal =
        (velocity.y < 0.0f) ? glm::vec3(0, 1, 0) : glm::vec3(0, -1, 0);
  } else {
    result.normal =
        (velocity.z < 0.0f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 0, -1);
  }

  return result;
}

bool VoxelCollider::CheckPoint(const VoxelShape& shape,
                               const glm::vec3& shape_pos,
                               const glm::vec3& point) {
  AABB root = shape.GetRootBounds().Translate(point);

  if (!root.Contains(point)) {
    return false;
  }

  for (const auto& boxes : shape.GetBoxes()) {
    if (boxes.Contains(point)) {
      return true;
    }
  }

  return false;
}

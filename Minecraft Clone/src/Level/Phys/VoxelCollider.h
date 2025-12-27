#pragma once
#include <glm/vec3.hpp>

class VoxelShape;
class AABB;

class VoxelCollider {
 public:
  struct CollisionResult {
    glm::vec3 hit_depth = {0.0f, 0.0f, 0.0f};
    bool is_hit = false;
  };

  struct CollisionSweptResult {
    bool is_hit = false;
    float time = 1.0f;  // 0.0 (start of tick) 1.0 means no hit
    glm::vec3 normal = {0.0f, 0.0f, 0.0f};  // face that we hit
    glm::vec3 point = {0.0f, 0.0f, 0.0f};   // point of contact
  };

  VoxelCollider() = delete;

  static CollisionResult CheckCollision(const VoxelShape& shape_a,
                                        const glm::vec3& pos_a,
                                        const VoxelShape& shape_b,
                                        const glm::vec3& pos_b);

  static CollisionSweptResult CheckSweepCollision(
      const VoxelShape& moving_shape, const glm::vec3& moving_pos,
      const glm::vec3& moving_vel, const VoxelShape& static_shape,
      const glm::vec3& static_pos);

  static CollisionSweptResult SweepAABB(const AABB& moving_box,
                                        const glm::vec3& delta,
                                        const AABB& static_box);

  static bool CheckPoint(const VoxelShape& shape, const glm::vec3& shape_pos,
                         const glm::vec3& point);
};

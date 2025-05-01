#include "Level/World/CollusionDetector.h"

#include "Level/Block/Block.h"
#include "Level/Block/Blocks.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Entity/Collusion/Hitbox.h"
#include "Level/Entity/Entities.h"
#include "Level/Entity/Entity.h"
#include "Utils/Math/Ray/Ray.h"
#include "Utils/Math/vectorOperations.h"

CollusionDetector::CollusionDetector(ChunkMap* cache) : cache_{cache} {}
CollusionDetector::~CollusionDetector() = default;

float CollusionDetector::TraceSingleAxisCollision(glm::vec3 origin,
                                                  int direction,
                                                  int distance_test) {
  float displacement = 0;

  glm::ivec3 floored_pos(floor(origin.x), floor(origin.y), floor(origin.z));
  int move = 0;

  int axis = direction >> 1;

  if (direction & 0b1) {
    displacement = origin[axis] - floored_pos[axis];
    move = -1;
  } else {
    displacement = 1 - (origin[axis] - floored_pos[axis]);
    move = 1;
  }

  for (int i = 0; i < distance_test; i++) {
    floored_pos[axis] += move * (i != 0);

    BlockID b =
        cache_->GetBlock(BlockPos{floored_pos.x, floored_pos.y, floored_pos.z});

    if (g_blocks.GetBlockType(b)->properties_->is_solid_) {
      return static_cast<float>(i) + displacement - 1.0f;
    }
  }

  return -1.f;
}

glm::dvec3 CollusionDetector::ComputeCollisionTimes(Entity* entity) {
  AABB hitbox = g_entity_list.GetEntity(entity->properties_.type_)->GetHitbox();

  glm::vec3 hitbox_start = entity->properties_.position_ - (hitbox.size_ / 2.f);
  glm::vec3 hitbox_end = entity->properties_.position_ + (hitbox.size_ / 2.f);

  int ix = static_cast<int>(floor(hitbox.size_.x)) + 1;
  int iy = static_cast<int>(floor(hitbox.size_.y)) + 1;
  int iz = static_cast<int>(floor(hitbox.size_.z)) + 1;

  glm::ivec3 i_bound = glm::ivec3(ix, iy, iz);

  glm::vec3 least_time(-1.f, -1.f, -1.f);

  float least_distance = static_cast<float>(kSearchDistance);

  for (int axis = 0; axis < 3; axis++) {
    // 0 = x; 1 = y; 2 = z

    int u_axis = (axis + 1) % 3;
    int v_axis = (axis + 2) % 3;

    for (int u = 0; u <= i_bound[u_axis]; u++) {
      for (int v = 0; v <= i_bound[v_axis]; v++) {
        glm::ivec3 offset(0, 0, 0);

        offset[axis] =
            i_bound[axis] * (entity->properties_.velocity_[axis] >= 0);
        offset[u_axis] = u;
        offset[v_axis] = v;

        glm::vec3 origin_ = hitbox_start + (glm::vec3)offset;

        if (origin_.x > hitbox_end.x) origin_.x = hitbox_end.x;
        if (origin_.y > hitbox_end.y) origin_.y = hitbox_end.y;
        if (origin_.z > hitbox_end.z) origin_.z = hitbox_end.z;

        bool is_point_on_hitbox_surface =
            (origin_.x == hitbox_start.x) || (origin_.x == hitbox_end.x) ||
            (origin_.y == hitbox_start.y) || (origin_.y == hitbox_end.y) ||
            (origin_.z == hitbox_start.z) || (origin_.z == hitbox_end.z);

        if (!is_point_on_hitbox_surface)  // Checks if the origin is on
                                          // the surface to optimize
                                          // stuff
          continue;

        if (entity->properties_.velocity_[axis] ==
            0.f)  // First checks if the velocity isn't 0 because if it
                  // is 0, it's not moving in that axis so it's not
                  // going to collide in that direction
          continue;

        int direction =
            axis * 2 +
            (entity->properties_.velocity_[axis] <
             0);  // The "+1" indicates that the direction is negative

        float distance = TraceSingleAxisCollision(
            origin_, direction, static_cast<int>(floor(least_distance)) + 2);

        if ((distance < least_distance) && (distance != -1.f))
          least_distance = distance;

        if (distance == -1.f)  // -1.f means that it cannot find any blocks that
                               // could collide in that range (5)
          continue;

        float time = abs(
            distance /
            entity->properties_.velocity_[axis]);  // This gets the time it
                                                   // takes for the entity to
                                                   // travel that distance

        if (time < least_time[axis]) {
          least_time[axis] = time;
          continue;
        }

        if (least_time[axis] == -1.f)  // leasttime[axis] == -1.f means that a
                                       // "time" value haven't been inputted yet
          least_time[axis] = time;
      }
    }
  }

  return least_time;
}

bool CollusionDetector::CheckRayIntersection(Ray& ray) {
  // Direction with magnitude
  glm::vec3 delta = ray.direction_;

  // Direction to Step
  glm::ivec3 direction = Sign(delta);

  // Location in grid
  glm::ivec3 blockPos = glm::ivec3(floor(ray.origin_.x), floor(ray.origin_.y),
                                   floor(ray.origin_.z));

  // To keep track of point location
  glm::vec3 end_point = ray.origin_;

  glm::vec3 delta_dist(abs(1 / delta[0]), abs(1 / delta[1]), abs(1 / delta[2]));

  // Stepping Variable
  glm::vec3 side_dist((static_cast<float>(direction[0]) *
                           (static_cast<float>(blockPos[0]) - end_point[0]) +
                       (static_cast<float>(direction[0]) * 0.5f) + 0.5f) *
                          delta_dist[0],
                      (static_cast<float>(direction[1]) *
                           (static_cast<float>(blockPos[1]) - end_point[1]) +
                       (static_cast<float>(direction[1]) * 0.5f) + 0.5f) *
                          delta_dist[1],
                      (static_cast<float>(direction[2]) *
                           (static_cast<float>(blockPos[2]) - end_point[2]) +
                       (static_cast<float>(direction[2]) * 0.5f) + 0.5f) *
                          delta_dist[2]);

  // Max Iterations
  static constexpr uint32_t kMaxIterations = 50;
  uint32_t iterations = 0;

  glm::bvec3 mask(false, false, false);

  while (iterations < kMaxIterations) {
    iterations++;

    BlockID b = cache_->GetBlock(BlockPos{blockPos.x, blockPos.y, blockPos.z});

    if (g_blocks.GetBlockType(b)->properties_->is_solid_) {
      ray.end_point_ = (glm::vec3)blockPos;

      ray.length_ = sqrtf(powf(ray.end_point_.x - ray.origin_.x, 2) +
                          powf(ray.end_point_.y - ray.origin_.y, 2) +
                          powf(ray.end_point_.z - ray.origin_.z, 2));

      for (int axis = 0; axis < 3; axis++) {
        if (mask[axis]) {
          ray.bounce_surface_ =
              delta[axis] < 0 ? axis * 2 + 1
                              : axis * 2;  // Set the surface it bounces off. +
                                           // 1 means that the surface is
                                           // behind, bottom, etc
          return true;
        }
      }
      return false;
    }

    glm::bvec3 l1 = LessThan(side_dist[0], side_dist[1], side_dist[2],
                             side_dist[1], side_dist[2], side_dist[0]);
    glm::bvec3 l2 = LessThanEqual(side_dist[0], side_dist[1], side_dist[2],
                                  side_dist[2], side_dist[0], side_dist[1]);

    mask[0] = l1[0] && l2[0];
    mask[1] = l1[1] && l2[1];
    mask[2] = l1[2] && l2[2];

    side_dist[0] += delta_dist[0] * mask[0];
    side_dist[1] += delta_dist[1] * mask[1];
    side_dist[2] += delta_dist[2] * mask[2];

    blockPos[0] += direction[0] * mask[0];
    blockPos[1] += direction[1] * mask[1];
    blockPos[2] += direction[2] * mask[2];
  }

  return false;
}

bool CollusionDetector::IsEntityOnGround(Entity* entity) {
  AABB hitbox = g_entity_list.GetEntity(entity->properties_.type_)->GetHitbox();

  glm::vec3 hitbox_start = entity->properties_.position_ - (hitbox.size_ / 2.f);
  glm::vec3 hitbox_end = entity->properties_.position_ + (hitbox.size_ / 2.f);

  int ix = static_cast<int>(floor(hitbox.size_.x)) + 1;
  int iz = static_cast<int>(floor(hitbox.size_.z)) + 1;

  float least_length = static_cast<float>(kSearchDistance);

  static constexpr float kOnGroundError = 0.001f;

  if (entity->properties_.velocity_.y > 0.f) {
    return false;
  }

  for (int x = 0; x <= ix; x++) {
    for (int z = 0; z <= iz; z++) {
      glm::vec3 origin = hitbox_start + glm::vec3(x, 0, z);

      if (origin.x > hitbox_end.x) origin.x = hitbox_end.x;
      if (origin.z > hitbox_end.z) origin.z = hitbox_end.z;

      float distance = 0.0f;

      // Set the distance to check to the previose least length from
      // collusion to optimize searching
      distance =
          TraceSingleAxisCollision(origin, Directions<BlockPos>::kDown,
                                   static_cast<int>(floor(least_length)) + 2);

      if (distance < least_length) {
        least_length = distance;
      }

      if ((distance != -1) && (distance < kOnGroundError)) {
        return true;
      }
    }
  }
  return false;
}

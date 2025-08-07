#include "Level/World/CollusionDetector.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Block.h"
#include "Level/Block/Blocks.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Entity/Collusion/Hitbox.h"
#include "Level/Entity/Entities.h"
#include "Level/Entity/Entity.h"
#include "Utils/Math/Ray/Ray.h"
#include "Utils/Math/vectorOperations.h"

CollusionDetector::CollusionDetector(GameContext& context, ChunkMap* cache)
    : context_{context}, cache_{cache} {}
CollusionDetector::~CollusionDetector() = default;

float CollusionDetector::TraceSingleAxisCollision(glm::vec3 origin,
                                                  int direction,
                                                  int distance_test) {
  const std::vector<BlockProperties>& properties =
      context_.blocks_->GetBlockPropertyList();

  float displacement = 0;

  BlockPos floored_pos(floor(origin.x), floor(origin.y), floor(origin.z));
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
    if (i != 0) {
      floored_pos[axis] += move;
    }

    BlockID b = cache_->GetBlock(floored_pos);

    if (properties[b].is_solid_) {
      return static_cast<float>(i) + displacement - 1.0f;
    }
  }

  return -1.f;
}

glm::dvec3 CollusionDetector::ComputeCollisionTimes(Entity* entity) {
  AABB hitbox = context_.entities_list_->GetEntity(entity->properties_.type_)
                    ->GetHitbox();

  glm::vec3 hitbox_start = entity->properties_.position_ - (hitbox.size_ / 2.f);
  glm::vec3 hitbox_end = entity->properties_.position_ + (hitbox.size_ / 2.f);

  int ix = static_cast<int>(floor(hitbox.size_.x)) + 1;
  int iy = static_cast<int>(floor(hitbox.size_.y)) + 1;
  int iz = static_cast<int>(floor(hitbox.size_.z)) + 1;

  glm::ivec3 i_bound{ix, iy, iz};

  glm::vec3 least_time{-1.0f, -1.0f, -1.0f};

  float least_distance = static_cast<float>(kSearchDistance);

  for (int axis = 0; axis < 3; axis++) {
    // 0 = x; 1 = y; 2 = z

    int u_axis = (axis + 1) % 3;
    int v_axis = (axis + 2) % 3;

    for (auto [u, v] : Product<2>({i_bound[u_axis] + 1, i_bound[v_axis] + 1})) {
      glm::ivec3 offset;

      offset[axis] = i_bound[axis] * (entity->properties_.velocity_[axis] >= 0);
      offset[u_axis] = u;
      offset[v_axis] = v;

      glm::vec3 origin = hitbox_start + (glm::vec3)offset;

      if (origin.x > hitbox_end.x) origin.x = hitbox_end.x;
      if (origin.y > hitbox_end.y) origin.y = hitbox_end.y;
      if (origin.z > hitbox_end.z) origin.z = hitbox_end.z;

      bool is_point_on_hitbox_surface =
          (origin.x == hitbox_start.x) || (origin.x == hitbox_end.x) ||
          (origin.y == hitbox_start.y) || (origin.y == hitbox_end.y) ||
          (origin.z == hitbox_start.z) || (origin.z == hitbox_end.z);

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
          axis * 2 + (entity->properties_.velocity_[axis] <
                      0);  // The "+1" indicates that the direction is negative

      float distance = TraceSingleAxisCollision(
          origin, direction, static_cast<int>(floor(least_distance)) + 2);

      if ((distance < least_distance) && (distance != -1.f))
        least_distance = distance;

      if (distance == -1.f)  // -1.f means that it cannot find any blocks that
                             // could collide in that range (5)
        continue;

      float time =
          abs(distance /
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

  return least_time;
}

bool CollusionDetector::CheckRayIntersection(Ray& ray) {
  // Direction with magnitude
  glm::vec3 delta = ray.direction_;
  glm::ivec3 direction = Sign(delta);
  glm::ivec3 block_pos = glm::ivec3(floor(ray.origin_.x), floor(ray.origin_.y),
                                    floor(ray.origin_.z));

  // To keep track of point location
  glm::vec3 end_point = ray.origin_;
  glm::vec3 delta_dist(abs(1 / delta[0]), abs(1 / delta[1]), abs(1 / delta[2]));

  // Stepping Variable
  glm::vec3 side_dist((static_cast<float>(direction[0]) *
                           (static_cast<float>(block_pos[0]) - end_point[0]) +
                       (static_cast<float>(direction[0]) * 0.5f) + 0.5f) *
                          delta_dist[0],
                      (static_cast<float>(direction[1]) *
                           (static_cast<float>(block_pos[1]) - end_point[1]) +
                       (static_cast<float>(direction[1]) * 0.5f) + 0.5f) *
                          delta_dist[1],
                      (static_cast<float>(direction[2]) *
                           (static_cast<float>(block_pos[2]) - end_point[2]) +
                       (static_cast<float>(direction[2]) * 0.5f) + 0.5f) *
                          delta_dist[2]);

  static constexpr uint32_t kMaxIterations = 50;
  uint32_t iterations = 0;

  glm::bvec3 mask(false, false, false);

  while (iterations < kMaxIterations) {
    iterations++;

    BlockID b =
        cache_->GetBlock({block_pos.x, block_pos.y, block_pos.z});

    if (context_.blocks_->GetBlockType(b)->properties_->is_solid_) {
      ray.end_point_ = (glm::vec3)block_pos;

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

    block_pos[0] += direction[0] * mask[0];
    block_pos[1] += direction[1] * mask[1];
    block_pos[2] += direction[2] * mask[2];
  }

  return false;
}

bool CollusionDetector::IsEntityOnGround(Entity* entity) {
  static constexpr int search_buffer = 2;
  AABB hitbox = context_.entities_list_->GetEntity(entity->properties_.type_)
                    ->GetHitbox();

  glm::vec3 hitbox_start = entity->properties_.position_ - (hitbox.size_ / 2.f);
  glm::vec3 hitbox_end = entity->properties_.position_ + (hitbox.size_ / 2.f);

  int ix = static_cast<int>(floor(hitbox.size_.x)) + 1;
  int iz = static_cast<int>(floor(hitbox.size_.z)) + 1;

  float least_length = static_cast<float>(kSearchDistance);

  if (entity->properties_.velocity_.y > 0.f) {
    return false;
  }

  for (auto [x, z] : Product<2>({ix + 1, iz + 1})) {
    glm::vec3 offset(x, 0.0f, z);
    glm::vec3 origin = hitbox_start + offset;

    if (origin.x > hitbox_end.x) origin.x = hitbox_end.x;
    if (origin.z > hitbox_end.z) origin.z = hitbox_end.z;

    float distance = 0.0f;

    // Set the distance to check to the previose least length from
    // collusion to optimize searching
    distance = TraceSingleAxisCollision(
        origin, kDownDirection,
        static_cast<int>(floor(least_length)) + search_buffer);

    least_length = std::min(distance, least_length);

    if ((distance != -1) && (distance < kOnGroundError)) {
      return true;
    }
  }
  return false;
}

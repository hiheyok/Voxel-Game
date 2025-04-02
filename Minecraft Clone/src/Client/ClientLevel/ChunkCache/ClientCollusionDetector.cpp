#include "Client/ClientLevel/ChunkCache/ClientCollusionDetector.h"
#include "Client/ClientLevel/ChunkCache/ClientChunkCache.h"
#include "Level/Chunk/Block/Block.h"
#include "Level/Chunk/Block/Blocks.h"
#include "Level/Entity/Entity.h"
#include "Level/Entity/Entities.h"
#include "Level/Entity/Collusion/Hitbox.h"
#include "Utils/Math/vectorOperations.h"
#include "Utils/Math/Ray/Ray.h"

ClientCollusionDetector::ClientCollusionDetector(ClientChunkCache* cache) : cache_{cache} {}
ClientCollusionDetector::~ClientCollusionDetector() = default;

float ClientCollusionDetector::GetDistanceUntilCollusionSingleDirection(glm::vec3 origin, int direction, int distanceTest) {
    float displacement = 0;

    glm::ivec3 flooredPos(floor(origin.x), floor(origin.y), floor(origin.z));
    int move = 0;

    int axis_ = direction >> 1;

    if (direction & 0b1) {
        displacement = origin[axis_] - (float)flooredPos[axis_];
        move = -1;
    }
    else {
        displacement = 1 - (origin[axis_] - (float)flooredPos[axis_]);
        move = 1;
    }

    for (int i = 0; i < distanceTest; i++) {

        flooredPos[axis_] += move * (int)(i != 0);

        BlockID b = cache_->GetBlock(BlockPos{ flooredPos.x, flooredPos.y, flooredPos.z });

        if (g_blocks.GetBlockType(b)->properties_->is_solid_) {
            return (float)i + displacement - 1.f;
        }
    }

    return -1.f;
}

glm::dvec3 ClientCollusionDetector::GetTimeTillCollusion(Entity* entity) {
    AABB hitbox = g_entity_list.GetEntity(entity->properties_.type_)->GetHitbox();

    glm::vec3 hitboxStart = entity->properties_.position_ - (hitbox.size_ / 2.f);
    glm::vec3 hitboxEnd = entity->properties_.position_ + (hitbox.size_ / 2.f);

    int ix = (int)floor(hitbox.size_.x) + 1;
    int iy = (int)floor(hitbox.size_.y) + 1;
    int iz = (int)floor(hitbox.size_.z) + 1;

    glm::ivec3 i_bound = glm::ivec3(ix, iy, iz);

    glm::vec3 leastTime(-1.f, -1.f, -1.f);

    int searchDistance = 5;

    float leastDistance = (float)searchDistance;

    for (int axis_ = 0; axis_ < 3; axis_++) {

        //0 = x; 1 = y; 2 = z

        int u_axis = (axis_ + 1) % 3;
        int v_axis = (axis_ + 2) % 3;

        for (int u = 0; u <= i_bound[u_axis]; u++) {
            for (int v = 0; v <= i_bound[v_axis]; v++) {
                glm::ivec3 offset(0, 0, 0);

                offset[axis_] = i_bound[axis_] * (entity->properties_.velocity_[axis_] >= 0);
                offset[u_axis] = u;
                offset[v_axis] = v;

                glm::vec3 origin_ = hitboxStart + (glm::vec3)offset;

                if (origin_.x > hitboxEnd.x)
                    origin_.x = hitboxEnd.x;
                if (origin_.y > hitboxEnd.y)
                    origin_.y = hitboxEnd.y;
                if (origin_.z > hitboxEnd.z)
                    origin_.z = hitboxEnd.z;

                bool IsPointOnHitboxSurface =
                    (origin_.x == hitboxStart.x) ||
                    (origin_.x == hitboxEnd.x) ||
                    (origin_.y == hitboxStart.y) ||
                    (origin_.y == hitboxEnd.y) ||
                    (origin_.z == hitboxStart.z) ||
                    (origin_.z == hitboxEnd.z);

                if (!IsPointOnHitboxSurface) //Checks if the origin is on the surface to optimize stuff
                    continue;

                if (entity->properties_.velocity_[axis_] == 0.f) //First checks if the velocity isn't 0 because if it is 0, it's not moving in that axis so it's not going to collide in that direction
                    continue;

                int direction = axis_ * 2 + (entity->properties_.velocity_[axis_] < 0); // The "+1" indicates that the direction is negative 

                float distance = GetDistanceUntilCollusionSingleDirection(origin_, direction, (int)floor(leastDistance) + 2);

                if ((distance < leastDistance) && (distance != -1.f))
                    leastDistance = distance;

                if (distance == -1.f) // -1.f means that it cannot find any blocks that could collide in that range (5)
                    continue;

                float time = abs(distance / entity->properties_.velocity_[axis_]);// This gets the time it takes for the entity to travel that distance

                if (time < leastTime[axis_]) {
                    leastTime[axis_] = time;
                    continue;
                }

                if (leastTime[axis_] == -1.f) //leasttime[axis] == -1.f means that a "time" value haven't been inputted yet
                    leastTime[axis_] = time;
            }
        }
    }

    return leastTime;
}

bool ClientCollusionDetector::CheckRayIntersection(Ray& ray) {

    //Direction with magnitude
    glm::vec3 delta = ray.direction_;

    //Direction to Step
    glm::ivec3 direction = Sign(delta);

    //Location in grid
    glm::ivec3 blockPos = glm::ivec3(floor(ray.origin_.x), floor(ray.origin_.y), floor(ray.origin_.z));

    //To keep track of point location
    glm::vec3 endPoint = ray.origin_;

    glm::vec3 deltaDist(
        abs(1 / delta[0]), abs(1 / delta[1]), abs(1 / delta[2])
    );

    //Stepping Variable
    glm::vec3 sideDist(
        ((float)direction[0] * ((float)blockPos[0] - endPoint[0]) + ((float)direction[0] * 0.5f) + 0.5f) * deltaDist[0],
        ((float)direction[1] * ((float)blockPos[1] - endPoint[1]) + ((float)direction[1] * 0.5f) + 0.5f) * deltaDist[1],
        ((float)direction[2] * ((float)blockPos[2] - endPoint[2]) + ((float)direction[2] * 0.5f) + 0.5f) * deltaDist[2]
    );

    //Max Iterations
    const uint32_t maxIterations = 50;
    uint32_t iterations = 0;

    glm::bvec3 mask(false, false, false);

    while (iterations < maxIterations) {
        iterations++;

        BlockID b = cache_->GetBlock(BlockPos{ blockPos.x, blockPos.y, blockPos.z });

        if (g_blocks.GetBlockType(b)->properties_->is_solid_) {

            ray.end_point_ = (glm::vec3)blockPos;

            ray.length_ = sqrtf(powf(ray.end_point_.x - ray.origin_.x, 2) + powf(ray.end_point_.y - ray.origin_.y, 2) + powf(ray.end_point_.z - ray.origin_.z, 2));

            for (int axis_ = 0; axis_ < 3; axis_++) {
                if (mask[axis_]) {
                    ray.bounce_surface_ = delta[axis_] < 0 ? axis_ * 2 + 1 : axis_ * 2; //Set the surface it bounces off. + 1 means that the surface is behind, bottom, etc
                    return true;
                }
            }
            return false;
        }

        glm::bvec3 l1 = LessThan(sideDist[0], sideDist[1], sideDist[2], sideDist[1], sideDist[2], sideDist[0]);
        glm::bvec3 l2 = LessThanEqual(sideDist[0], sideDist[1], sideDist[2], sideDist[2], sideDist[0], sideDist[1]);

        mask[0] = l1[0] && l2[0];
        mask[1] = l1[1] && l2[1];
        mask[2] = l1[2] && l2[2];

        sideDist[0] += (float)mask[0] * deltaDist[0];
        sideDist[1] += (float)mask[1] * deltaDist[1];
        sideDist[2] += (float)mask[2] * deltaDist[2];

        blockPos[0] += ((int)mask[0]) * direction[0];
        blockPos[1] += ((int)mask[1]) * direction[1];
        blockPos[2] += ((int)mask[2]) * direction[2];
    }

    return false;
}

bool ClientCollusionDetector::isEntityOnGround(Entity* entity) {
    AABB hitbox = g_entity_list.GetEntity(entity->properties_.type_)->GetHitbox();

    glm::vec3 hitboxStart = entity->properties_.position_ - (hitbox.size_ / 2.f);
    glm::vec3 hitboxEnd = entity->properties_.position_ + (hitbox.size_ / 2.f);

    int ix = (int)floor(hitbox.size_.x) + 1;
    int iz = (int)floor(hitbox.size_.z) + 1;

    int searchDistance = 5;

    float leastLength = (float)searchDistance;

    float onGroundError = 0.001f;

    if (entity->properties_.velocity_.y > 0.f) {
        return false;
    }

    for (int x = 0; x <= ix; x++) {
        for (int z = 0; z <= iz; z++) {
            glm::vec3 origin = hitboxStart + glm::vec3(x, 0, z);

            if (origin.x > hitboxEnd.x)
                origin.x = hitboxEnd.x;
            if (origin.z > hitboxEnd.z)
                origin.z = hitboxEnd.z;

            float distance = 0.f;

            //Set the distance to check to the previose least length from collusion to optimize searching
            distance = GetDistanceUntilCollusionSingleDirection(origin, NY, (int)floor(leastLength) + 2);

            if (distance < leastLength) {
                leastLength = distance;
            }

            if ((distance != -1) && (distance < onGroundError)) {
                return true;
            }

        }
    }
    return false;
}
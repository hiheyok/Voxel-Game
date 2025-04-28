#pragma once
#include <glm/vec3.hpp>

class ChunkMap;

struct Entity;
struct Ray;

class CollusionDetector {
   public:
    CollusionDetector(ChunkMap* cache);
    ~CollusionDetector();

    bool CheckRayIntersection(Ray& ray);
    float TraceSingleAxisCollision(glm::vec3 Origin, int direction,
                                   int distanceTest);
    glm::dvec3 ComputeCollisionTimes(Entity* entity);
    bool IsEntityOnGround(Entity* entity);

   private:
    ChunkMap* cache_;
};
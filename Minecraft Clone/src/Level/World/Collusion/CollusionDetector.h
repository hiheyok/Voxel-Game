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

    float GetDistanceUntilCollusionSingleDirection(glm::vec3 Origin, int direction, int distanceTest);

    glm::dvec3 GetTimeTillCollusion(Entity* entity);

    bool isEntityOnGround(Entity* entity);
private:
    ChunkMap* cache_;
};
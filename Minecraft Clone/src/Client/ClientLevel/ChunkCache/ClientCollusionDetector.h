#pragma once
#include <glm/vec3.hpp>

class ClientChunkCache;

struct Entity;
struct Ray;

class ClientCollusionDetector {
public:
    ClientCollusionDetector(ClientChunkCache* cache);
    ~ClientCollusionDetector();

    bool CheckRayIntersection(Ray& ray);

    float GetDistanceUntilCollusionSingleDirection(glm::vec3 Origin, int direction, int distanceTest);

    glm::dvec3 GetTimeTillCollusion(Entity* entity);

    bool isEntityOnGround(Entity* entity);
private:
    ClientChunkCache* cache_;
};
#pragma once
#include <glm/vec3.hpp>

class ChunkMap;
class GameContext;

struct Entity;
struct Ray;

class CollusionDetector {
 public:
  CollusionDetector(GameContext&, ChunkMap* cache);
  ~CollusionDetector();

  bool CheckRayIntersection(Ray& ray);
  float TraceSingleAxisCollision(glm::vec3 Origin, int direction,
                                 int distanceTest);
  glm::dvec3 ComputeCollisionTimes(Entity* entity);
  bool IsEntityOnGround(Entity* entity);

 private:
  static constexpr int kSearchDistance = 5;
  static constexpr float kOnGroundError = 0.001f;
  GameContext& context_;
  ChunkMap* cache_;
};

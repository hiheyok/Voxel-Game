#include <cmath>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#define _CRTDBG_MAP_ALLOC
double toRadians(double x) { return x * 0.01745329251; }

float FindDistanceNoSqrt(float x, float y, float z, float x1, float y1,
                         float z1) {
  return (x - x1) * (x - x1) + (y - y1) * (y - y1) + (z - z1) * (z - z1);
}

double FindDistanceNoSqrt(int x, int y, int x1, int y1) {
  return pow(x - x1, 2) + pow(y - y1, 2);
}

double FindDistance(int x, int y, int z, int x1, int y1, int z1) {
  return sqrt(pow(x - x1, 2) + pow(y - y1, 2) + pow(z - z1, 2));
}

double FindDistance(int x, int y, int x1, int y1) {
  return sqrt(pow(x - x1, 2) + pow(y - y1, 2));
}

template <typename T>
double FindDistance(T x, T x1) {
  return sqrt(pow(static_cast<double>(x) - x1, 2));
}

double FindDistance(glm::ivec3 pos0, glm::ivec3 pos1) {
  return sqrt(pow(static_cast<double>(pos0.x) - pos1.x, 2) +
              pow(static_cast<double>(pos0.y) - pos1.y, 2) +
              pow(static_cast<double>(pos0.z) - pos1.z, 2));
}

double FindDistance(glm::ivec2 pos0, glm::ivec2 pos1) {
  return sqrt(pow(static_cast<double>(pos0.x) - pos1.x, 2) +
              pow(static_cast<double>(pos0.y) - pos1.y, 2));
}

double FindDistance(glm::vec3 pos0, glm::vec3 pos1) {
  return sqrt(pow(static_cast<double>(pos0.x) - pos1.x, 2) +
              pow(static_cast<double>(pos0.y) - pos1.y, 2) +
              pow(static_cast<double>(pos0.z) - pos1.z, 2));
}

double FindDistance(glm::vec2 pos0, glm::vec2 pos1) {
  return sqrt(pow(static_cast<double>(pos0.x) - pos1.x, 2) +
              pow(static_cast<double>(pos0.y) - pos1.y, 2));
}

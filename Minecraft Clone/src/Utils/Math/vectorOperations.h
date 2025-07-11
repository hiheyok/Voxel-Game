// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <cmath>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

inline static glm::ivec3 Sign(glm::vec3 vector) {
  glm::ivec3 signs(0, 0, 0);

  signs[0] = (vector[0] > 0) - (vector[0] < 0);
  signs[1] = (vector[1] > 0) - (vector[1] < 0);
  signs[2] = (vector[2] > 0) - (vector[2] < 0);

  return signs;
}

inline static glm::bvec3 LessThan(glm::vec3 vec1, glm::vec3 vec2) {
  glm::bvec3 less(false, false, false);

  less[0] = vec1[0] < vec2[0];
  less[1] = vec1[1] < vec2[1];
  less[2] = vec1[2] < vec2[2];

  return less;
}

inline static glm::bvec3 LessThan(float x, float y, float z, float x1, float y1,
                                  float z1) {
  glm::bvec3 less(false, false, false);

  less[0] = x < x1;
  less[1] = y < y1;
  less[2] = z < z1;

  return less;
}

inline static glm::bvec3 LessThanEqual(glm::vec3 vec1, glm::vec3 vec2) {
  glm::bvec3 less(false, false, false);

  less[0] = vec1[0] <= vec2[0];
  less[1] = vec1[1] <= vec2[1];
  less[2] = vec1[2] <= vec2[2];

  return less;
}

inline static glm::bvec3 LessThanEqual(float x, float y, float z, float x1,
                                       float y1, float z1) {
  glm::bvec3 less(false, false, false);

  less[0] = x <= x1;
  less[1] = y <= y1;
  less[2] = z <= z1;

  return less;
}

// TODO: Overload std::min ?
inline static glm::bvec3 Min(glm::vec3 vec1, glm::vec3 vec2) {
  vec1[0] = vec1[0] < vec2[0] ? vec1[0] : vec2[0];
  vec1[1] = vec1[1] < vec2[1] ? vec1[1] : vec2[1];
  vec1[2] = vec1[2] < vec2[2] ? vec1[2] : vec2[2];

  return vec1;
}

inline static glm::bvec3 Min(float x, float y, float z, float x1, float y1,
                             float z1) {
  glm::vec3 vec1(0.f, 0.f, 0.f);

  vec1[0] = x < x1 ? x : x1;
  vec1[1] = y < y1 ? y : y1;
  vec1[2] = z < z1 ? z : z1;

  return vec1;
}

inline static float Magnitude(glm::vec3 v) {
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline static float Magnitude(glm::vec2 v) {
  return sqrtf(v.x * v.x + v.y * v.y);
}

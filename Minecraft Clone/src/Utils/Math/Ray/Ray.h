#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
struct Ray {
  float length_ = 0;

  glm::vec3 direction_ = {0.0f, 0.0f, 0.0f};
  glm::vec3 origin_ = {0.0f, 0.0f, 0.0f};
  glm::vec3 end_point_ = {0.0f, 0.0f, 0.0f};

  int bounce_surface_ = 0;
};

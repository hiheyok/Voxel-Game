#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
struct Ray {
  float length_ = 0;

  glm::vec3 direction_ = glm::vec3(0., 0., 0.);
  glm::vec3 origin_ = glm::vec3(0., 0., 0.);
  glm::vec3 end_point_ = glm::vec3(0., 0., 0.);

  int bounce_surface_ = 0;
};

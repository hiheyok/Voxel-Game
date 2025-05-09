// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <iostream>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Model {
struct ModelData {
  std::vector<float> vertices_;
  std::vector<uint32_t> indices_;

  int vertices_count_ = 0;
};

struct UV {
  glm::vec2 p0_ = glm::vec2(0.f, 0.f);
  glm::vec2 p1_ = glm::vec2(0.f, 0.f);
};

struct RectangularPrism {
  RectangularPrism(float x, float y, float z);

  RectangularPrism(glm::vec3 size, glm::vec3 offset);

  ModelData GetVertices();

  glm::vec3 offset_ = glm::vec3(0.f, 0.f, 0.f);

  glm::vec3 size_ = glm::vec3(0.f, 0.f, 0.f);

  UV uv_map_[6]{};
};
}  // namespace Model

class EntityModel {
 public:
  Model::RectangularPrism* AddRectangle(glm::vec3 size, glm::vec3 offset);

  Model::ModelData GetVertices();

  void Clear();

 private:
  std::vector<Model::RectangularPrism> shapes_;
};

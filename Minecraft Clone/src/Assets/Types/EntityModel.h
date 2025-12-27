// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "Assets/Asset.h"

namespace Model {
struct ModelData {
  std::vector<float> vertices_;
  std::vector<uint32_t> indices_;

  int vertices_count_ = 0;
};

struct UV {
  glm::vec2 p0_ = {0.0f, 0.0f};
  glm::vec2 p1_ = {0.0f, 0.0f};
};

struct RectangularPrism {
  RectangularPrism(float x, float y, float z);

  RectangularPrism(glm::vec3 size, glm::vec3 offset);

  ModelData GetVertices();

  glm::vec3 offset_ = {0.0f, 0.0f, 0.0f};

  glm::vec3 size_ = {0.0f, 0.0f, 0.0f};

  UV uv_map_[6]{};
};
}  // namespace Model

class EntityModel : public Asset {
 public:
  EntityModel(const std::string& key);

  Model::RectangularPrism* AddRectangle(glm::vec3 size, glm::vec3 offset);
  Model::ModelData GetVertices();

  void Clear();

  void Load() override {} // tmp

  std::string texture_id_; // tmp fix
  glm::vec3 hitbox_; // tmp fix

 private:
  std::vector<Model::RectangularPrism> shapes_;
};

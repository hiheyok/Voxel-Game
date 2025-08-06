// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <string>
#include <vector>
#include <glm/vec2.hpp>

struct GUIElement {
  struct GUIVertices {
    std::vector<float> vertices_ = {};
    std::vector<int> indices_ = {};
  };

  GUIElement();
  ~GUIElement();
  GUIElement(std::string text, glm::vec2 size, glm::vec2 location);
  GUIVertices GetVertices();

  std::string text_ = "";
  glm::vec2 size_ = {0.0f, 0.0f};
  glm::vec2 location_ = {0.0f, 0.0f};
  glm::vec2 uv_p0_ = {0.0f, 0.0f};
  glm::vec2 uv_p1_ = {0.0f, 0.0f};

  int texture_ = 0;
  bool has_texture_ = false;
  bool is_button_ = 0;

  size_t buffer_index_ = 0;
};

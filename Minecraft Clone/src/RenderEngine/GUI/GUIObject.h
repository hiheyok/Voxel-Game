#pragma once
#include <glm/vec2.hpp>
#include <string>
#include <vector>

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
  glm::vec2 size_ = glm::vec2(0.f, 0.f);
  glm::vec2 location_ = glm::vec2(0.f, 0.f);
  glm::vec2 uv_p0_ = glm::vec2(0.f, 0.f);
  glm::vec2 uv_p1_ = glm::vec2(0.f, 0.f);

  int texture_ = 0;
  bool has_texture_ = false;
  bool is_button_ = 0;

  size_t buffer_index_ = 0;
};

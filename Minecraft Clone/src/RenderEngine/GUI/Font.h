// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

static constexpr float CharWidth[256]{
    1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
    1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
    0.625f, 0.125f, 0.250f, 0.625f, 0.625f, 0.625f, 0.625f, 0.125f, 0.375f, 0.375f, 0.375f, 0.625f, 0.125f, 0.625f, 0.125f, 0.625f,
    0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.125f, 0.125f, 0.500f, 0.625f, 0.500f, 0.625f,
    0.750f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.375f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f,
    0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.375f, 0.625f, 0.375f, 0.625f, 0.625f,
    0.250f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.500f, 0.625f, 0.625f, 0.125f, 0.625f, 0.500f, 0.250f, 0.625f, 0.625f, 0.625f,
    0.625f, 0.625f, 0.625f, 0.625f, 0.375f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.375f, 0.125f, 0.375f, 0.625f, 1.000f,
    1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
    1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 0.625f, 1.000f, 1.000f, 0.625f,
    1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 0.500f, 0.500f, 1.000f, 1.000f, 0.625f, 1.000f, 1.000f, 1.000f, 0.750f, 0.750f,

    // Rows beyond this is just garbage data for now
    1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
    1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
    1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
    1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
    1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
};

class RenderableFont {
 public:
  void SetBackgroundPadding(float H, float W);
  void SetText(std::string str);
  void AddBackground(glm::vec3 color, float alpha);
  void FontSize(float size);
  void SetColor(glm::vec3 clr);
  void SetPosition(glm::vec2 pos);

  bool background_ = false;
  std::vector<float> GetVertices() const;  // -1 - 1 coords
  std::vector<float> GetBackgroundVertices() const;

 private:
  std::string render_text_ = "";
  float font_size_ = 0.0625f;
  float spacing_ = 0.125f;

  glm::vec4 GetCharUV(char c) const noexcept;
  glm::vec3 color_ = glm::vec3(1.f, 1.f, 1.f);
  glm::vec2 position_ = glm::vec2(-1.f, -1.f);
  glm::vec3 background_color_ = glm::vec3(1.f, 1.f, 1.f);
  float background_alpha_ = 1.f;
  mutable float text_length_ = 0.f; //TODO(hiheyok): fix this later
  float background_height_padding_ = 0.f;
  float background_width_padding_ = 0.f;

  std::vector<float> GetCharactorVertex(char c, float xOffset, float yOffset,
                                        float xOrigin, float yOrigin) const noexcept;
  float GetCharWidth(char c) const noexcept;
};

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <ostream>
#include <vector>

struct UIVertexFormat {
  float x_ = 0.0f, y_ = 0.0f;
  float u_ = 0.0f, v_ = 0.0f;
  float r_ = 1.0f, g_ = 1.0f, b_ = 1.0f, a_ = 0.0f;
  int tex_idx_ = -1; // -1 default for no texture
  friend std::ostream& operator<<(std::ostream& s, const UIVertexFormat& obj);
};

static_assert(sizeof(UIVertexFormat) == 4 * 9);

struct UIRectangle {
  UIRectangle() = default;
  UIRectangle(const UIRectangle&) = default;
  UIRectangle(UIRectangle&&) noexcept = default;

  UIRectangle& operator=(const UIRectangle&) = default;
  UIRectangle& operator=(UIRectangle&&) noexcept = default;

  void GetVertices(std::vector<UIVertexFormat>& vertices,
                   std::vector<uint32_t>& indices) const;

  glm::vec2 pos_;  // Coordinates
  glm::vec2 size_;
  glm::vec2 uv_beg_, uv_end_;
  glm::vec4 rgba_;  // TODO(hiheyok): later have it more specific to each corner
  int tex_idx_ = -1;
  uint64_t tex_handle_ = 0;
};

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

struct UIVertexFormat {
  float x_ = 0.0, y_ = 0.0;
  float r_ = 1.0, g_ = 1.0, b_ = 1.0, a_ = 0.0;
};

static_assert(sizeof(UIVertexFormat) == 4 * 6);

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
  bool use_texture_ = false;
};

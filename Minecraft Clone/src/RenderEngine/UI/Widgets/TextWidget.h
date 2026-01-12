// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

#include "RenderEngine/UI/Widgets/Widget.h"

class ColoredComponent;
class GameContext;
struct UIRectangle;

// Widget that renders text using the ASCII font atlas.
// Each character becomes a UIRectangle with appropriate UV coordinates.
// Optionally includes a background color.
class TextWidget : public Widget {
 public:
  explicit TextWidget(GameContext& context);
  ~TextWidget() override;

  void SetText(const std::string& text);
  void SetTextColor(glm::vec3 color);

  // Background settings
  void SetBackgroundColor(glm::vec4 rgba);
  float GetTextLength() const;

 protected:
  // Override to add text rectangles after component rectangles
  void GetPrimitives(std::vector<UIRectangle>& primitives) const override;

 private:
  // Text properties
  std::string text_;
  glm::vec3 color_ = {1.0f, 1.0f, 1.0f};
  float spacing_ = 1.0f;    // Extra spacing between characters
  uint64_t font_handle_ = 0;

  // Background
  ColoredComponent* background_component_ = nullptr;
  bool background_enabled_ = false;

  // Text rendering helpers
  float GetCharWidth(char c) const noexcept;
  glm::vec4 GetCharUV(char c) const noexcept;
};

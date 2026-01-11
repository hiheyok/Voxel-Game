#pragma once

#include <cassert>
#include <glm/vec4.hpp>
#include <vector>

#include "RenderEngine/UI/Components/Component.h"

struct UIRectangle;

class ColoredComponent : public Component {
 public:
  ColoredComponent(float r, float g, float b, float a = 1.0f);
  ColoredComponent(glm::vec4 rgba);
  ~ColoredComponent() override;

  void GetRectangles(std::vector<UIRectangle>& primitives) override;
  void SetColor(float r, float g, float b, float a) noexcept;
  void SetColor(glm::vec4 rgba) noexcept;

 private:
  float r_, g_, b_, a_;
};

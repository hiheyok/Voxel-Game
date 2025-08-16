#pragma once

#include <cassert>
#include <vector>

#include "RenderEngine/UI/Components/Component.h"

struct UIRectangle;

class ColoredComponent : public Component {
 public:
  ColoredComponent(float r, float g, float b, float a = 1.0f);
  ~ColoredComponent() override;

  void GetRectangles(std::vector<UIRectangle>& primitives) override;

 private:
  float r_, g_, b_, a_;
};

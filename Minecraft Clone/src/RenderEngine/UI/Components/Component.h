#pragma once

#include <cstdint>
#include <vector>

struct UIRectangle;

class Component {
 public:
  Component() = default;
  virtual ~Component() = default;

  virtual void GetRectangles(std::vector<UIRectangle>& primitives) = 0;
};

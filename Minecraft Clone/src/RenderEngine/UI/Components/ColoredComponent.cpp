#include "RenderEngine/UI/Components/ColoredComponent.h"

#include "RenderEngine/UI/Data/UserInterfaceData.h"

ColoredComponent::ColoredComponent(float r, float g, float b, float a)
    : Component{}, r_{r}, g_{g}, b_{b}, a_{a} {
  assert(0 <= r && r <= 1);
  assert(0 <= g && g <= 1);
  assert(0 <= b && b <= 1);
  assert(0 <= a && a <= 1);
};
ColoredComponent::~ColoredComponent() = default;

void ColoredComponent::GetRectangles(std::vector<UIRectangle>& primitives) {
  UIRectangle rect;
  rect.pos_.x = 0.0f;
  rect.pos_.y = 0.0f;
  rect.size_.x = 1.0f;
  rect.size_.y = 1.0f;

  rect.rgba_.r = r_;
  rect.rgba_.g = g_;
  rect.rgba_.b = b_;
  rect.rgba_.a = a_;

  primitives.emplace_back(rect);
}
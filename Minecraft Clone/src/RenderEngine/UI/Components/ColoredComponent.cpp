#include "RenderEngine/UI/Components/ColoredComponent.h"

#include <vector>

#include "RenderEngine/UI/Components/Component.h"
#include "RenderEngine/UI/Data/UserInterfaceData.h"
#include "Utils/Assert.h"

using std::vector;

ColoredComponent::ColoredComponent(float r, float g, float b, float a)
    : Component{}, r_{r}, g_{g}, b_{b}, a_{a} {
  GAME_ASSERT(0 <= r && r <= 1, "Red value out of range");
  GAME_ASSERT(0 <= g && g <= 1, "Green value out of range");
  GAME_ASSERT(0 <= b && b <= 1, "Blue value out of range");
  GAME_ASSERT(0 <= a && a <= 1, "Alpha value out of range");
};
ColoredComponent::~ColoredComponent() = default;

void ColoredComponent::GetRectangles(vector<UIRectangle>& primitives) {
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
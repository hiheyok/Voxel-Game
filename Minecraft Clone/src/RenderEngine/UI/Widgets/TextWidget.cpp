// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/UI/Widgets/TextWidget.h"

#include <cstddef>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2D.h"
#include "RenderEngine/UI/Components/ColoredComponent.h"
#include "RenderEngine/UI/Data/UserInterfaceData.h"

using std::make_unique;
using std::string;
using std::vector;

// Width in pixels
static constexpr uint8_t kCharWidth[256]{
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 5, 1, 2, 5, 5, 5, 5, 1, 3, 3, 3, 5, 1, 5,
    1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 4, 5, 4, 5, 6, 5, 5, 5, 5,
    5, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3,
    5, 3, 5, 5, 2, 5, 5, 5, 5, 5, 4, 5, 5, 1, 5, 4, 2, 5, 5, 5, 5, 5, 5,
    5, 3, 5, 5, 5, 5, 5, 5, 3, 1, 3, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 5, 8, 8, 5, 8,
    8, 8, 8, 8, 8, 4, 4, 8, 8, 5, 8, 8, 8, 6, 6,  // Beyond is garbage
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

TextWidget::TextWidget(GameContext& context) : Widget(context) {
  // Get font texture handle for bindless rendering
  RenderHandle<Texture2D> font_tex =
      context_.render_resource_manager_->GetTexture2D("ascii_font");
  font_handle_ = font_tex->GetHandle();
}

TextWidget::~TextWidget() = default;

void TextWidget::SetText(const string& text) { text_ = text; }

void TextWidget::SetTextColor(glm::vec3 color) { color_ = color; }

float TextWidget::GetCharWidth(char c) const noexcept {
  return static_cast<float>(kCharWidth[c]) / 8;
}

glm::vec4 TextWidget::GetCharUV(char c) const noexcept {
  int index = static_cast<int>(c);

  float width = GetCharWidth(c);

  float x0 = static_cast<float>(index & 0b1111);
  float y0 = static_cast<float>(index >> 4);

  float x1 = x0 + width;
  float y1 = y0 + 1;

  x0 = x0 / 16;
  y0 = y0 / 16;
  x1 = x1 / 16;
  y1 = y1 / 16;

  // Flip Y for OpenGL texture coordinates
  return glm::vec4{x0, 1.0f - y1, x1, 1.0f - y0};
}

void TextWidget::GetPrimitives(vector<UIRectangle>& primitives) const {
  // First call base to render any components (like background)
  Widget::GetPrimitives(primitives);

  // Then add text rectangles
  if (text_.empty()) return;

  // Height of widget
  float height = screen_.size_.y;

  float x_offset = screen_.pos_.x;
  float y_offset = screen_.pos_.y;

  if (primitives.max_size() < primitives.size() + text_.size()) {
    primitives.reserve(primitives.size() + text_.size());
  }

  for (char c : text_) {
    float char_width = GetCharWidth(c);
    glm::vec4 uv = GetCharUV(c);

    UIRectangle rect;
    rect.pos_.x = x_offset;
    rect.pos_.y = y_offset;
    rect.size_.x = char_width * height;
    rect.size_.y = 1.0f * height;

    rect.uv_beg_ = {uv.x, uv.y};
    rect.uv_end_ = {uv.z, uv.w};

    rect.rgba_ = {color_.x, color_.y, color_.z, 1.0f};
    rect.tex_handle_ = font_handle_;

    primitives.push_back(rect);

    x_offset += char_width * height + spacing_;
  }
}

void TextWidget::SetBackgroundColor(glm::vec4 rgba) {
  if (!background_component_) {
    auto bg = make_unique<ColoredComponent>(rgba.r, rgba.g, rgba.b, rgba.a);
    background_component_ = bg.get();
    components_.insert(components_.begin(), std::move(bg));
    background_enabled_ = true;
  } else {
    // Update existing background
    background_component_->SetColor(rgba);
  }
}

float TextWidget::GetTextLength() const {
  if (text_.empty()) return 0.0f;

  float height = screen_.size_.y;
  float total_width = 0.0f;

  for (char c : text_) {
    total_width += GetCharWidth(c);
  }

  // - 1 to remove the trailing spacing at the end
  total_width = total_width * height + spacing_ * (text_.size() - 1);

  return total_width;
}

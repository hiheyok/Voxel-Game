// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/UI/Widgets/Widget.h"

#include <algorithm>
#include <vector>

#include "RenderEngine/UI/Components/Component.h"
#include "RenderEngine/UI/Data/UserInterfaceData.h"
#include "RenderEngine/UI/UIRenderer.h"

Widget::Widget() : parent_{nullptr}, self_dirty_{true}, children_dirty_{true} {}
Widget::~Widget() = default;

void Widget::SetPivot(float x, float y) noexcept {
  pivot_.x = x;
  pivot_.y = y;
}

void Widget::SetAnchorBoth(float x, float y) noexcept {
  SetAnchorMax(x, y);
  SetAnchorMin(x, y);
}
void Widget::SetAnchorMax(float x, float y) noexcept {
  anchor_max_.x = x;
  anchor_max_.y = y;
}
void Widget::SetAnchorMin(float x, float y) noexcept {
  anchor_min_.x = x;
  anchor_min_.y = y;
}

void Widget::SetOffsetMax(float x, float y) noexcept {
  offset_max_.x = x;
  offset_max_.y = y;
}

void Widget::SetOffsetMin(float x, float y) noexcept {
  offset_min_.x = x;
  offset_min_.y = y;
}

void Widget::AddChildWidget(std::unique_ptr<Widget> widget) {
  if (!widget) {  // guard against nullptr
    return;
  }

  widget->SetScreenManager(manager_);
  widget->parent_ = this;
  children_.push_back(std::move(widget));
  SetDirty();
}

void Widget::AddComponent(std::unique_ptr<Component> component) {
  if (!component) {
    return;
  }
  components_.push_back(std::move(component));
}

void Widget::TryUpdateLayout(const UIRectangle& parent) {
  // Skip if entire branch is not dirty
  if (!self_dirty_ && !children_dirty_) {
    return;
  }

  if (self_dirty_) {
    if (anchor_max_ == anchor_min_) {
      glm::vec2 relative_size = offset_max_ - offset_min_;
      glm::vec2 absolute_size = parent.size_ * relative_size;
      glm::vec2 anchor_pos = parent.pos_ + anchor_min_ * parent.size_;
      glm::vec2 pivot_offset = pivot_ * absolute_size;
      screen_.pos_ = anchor_pos - pivot_offset + offset_min_;
      screen_.size_ = absolute_size;
    } else {
      glm::vec2 anchor_min_pos = parent.pos_ + anchor_min_ * parent.size_;
      glm::vec2 anchor_max_pos = parent.pos_ + anchor_max_ * parent.size_;
    
      glm::vec2 offset_min_pos = anchor_min_pos + offset_min_ * parent.size_;
      glm::vec2 offset_max_pos = anchor_min_pos - offset_max_ * parent.size_;
    
      screen_.size_ = offset_max_ - offset_min_;
      screen_.pos_ = offset_min_pos;
    }
  }

  if (self_dirty_ || children_dirty_) {
    for (auto& child : children_) {
      child->TryUpdateLayout(screen_);
    }
  }

  self_dirty_ = false;
  children_dirty_ = false;
}

void Widget::SubmitToRenderer(UIRenderer& renderer) {
  std::vector<UIVertexFormat> vertices;
  std::vector<uint32_t> indices;

  GetGeometry(vertices, indices);
  renderer.Submit(vertices, indices);

  for (auto& child : children_) {
    child->SubmitToRenderer(renderer);
  }
}

void Widget::AddCallback() {
  // TODO(hiheyok): Implement later
}

void Widget::SetDirty() {
  if (!self_dirty_) {
    self_dirty_ = true;
    SetChildDirty();
  }
}

void Widget::SetChildDirty() {
  children_dirty_ = true;
  if (parent_) {
    parent_->SetChildDirty();
  }
}

void Widget::SetScreenManager(ScreenManager* manager) noexcept {
  if (manager_ != manager) {
    manager_ = manager;

    for (auto& child : children_) {
      child->SetScreenManager(manager);
    }
  }
}

void Widget::GetGeometry(std::vector<UIVertexFormat>& vertices,
                         std::vector<uint32_t>& indices) {
  std::vector<UIRectangle> primitives;

  for (const auto& component : components_) {
    component->GetRectangles(primitives);
  }

  // Resize rectangles to match with the screen bounds
  glm::vec2 base_pos = screen_.pos_;
  glm::vec2 base_size = screen_.size_;

  for (auto& rect : primitives) {
    rect.pos_ = base_pos + base_size * rect.pos_;
    rect.size_ *= screen_.size_;
  }

  for (const auto& rect : primitives) {
    rect.GetVertices(vertices, indices);
  }
}

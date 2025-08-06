// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/UI/Widgets/Widget.h"

#include <algorithm>
#include <vector>

#include "RenderEngine/UI/Components/Component.h"
#include "RenderEngine/UI/Data/UserInterfaceData.h"
#include "RenderEngine/UI/UIRenderer.h"

Widget::Widget() : parent_{nullptr}, self_dirty_{true}, children_dirty_{true} {}
Widget::~Widget() = default;

void Widget::SetPivot(glm::vec2 pos) noexcept { pivot_ = pos; }

void Widget::SetAnchorBoth(glm::vec2 pos) noexcept {
  SetAnchorMax(pos);
  SetAnchorMin(pos);
}
void Widget::SetAnchorMax(glm::vec2 pos) noexcept { anchor_max_ = pos; }
void Widget::SetAnchorMin(glm::vec2 pos) noexcept { anchor_min_ = pos; }

void Widget::SetOffsetMax(glm::vec2 pos) noexcept { offset_max_ = pos; }

void Widget::SetOffsetMin(glm::vec2 pos) noexcept { offset_min_ = pos; }

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
    glm::vec2 abs_anchor_min = anchor_min_ * parent.size_;
    glm::vec2 abs_anchor_max = anchor_max_ * parent.size_;

    if (anchor_max_ == anchor_min_) {
      glm::vec2 widget_size = offset_max_ - offset_min_;
      glm::vec2 anchor_pos = parent.pos_ + abs_anchor_min;
      glm::vec2 abs_pivot_pos = pivot_ * widget_size;
      screen_.pos_ = anchor_pos - abs_pivot_pos + offset_min_;
      screen_.size_ = widget_size;
    } else {
      glm::vec2 start_pos = abs_anchor_min + offset_min_;
      glm::vec2 end_pos = abs_anchor_max - offset_max_;
      screen_.pos_ = start_pos;
      screen_.size_ = end_pos - start_pos;
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

void Widget::SetBranchDirty() noexcept {
  self_dirty_ = true;
  if (children_.size() != 0) {
    children_dirty_ = true;

    for (auto& child : children_) {
      child->SetBranchDirty();
    }
  }
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

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/UI/Widgets/Widget.h"

#include <algorithm>
#include <vector>

#include "RenderEngine/UI/Data/UIVertexFormat.h"

Widget::Widget() = default;
Widget::~Widget() = default;
Widget::Widget(Widget&&) noexcept = default;
Widget& Widget::operator=(Widget&&) noexcept = default;

std::vector<UIVertexFormat> Widget::GetVertices() const {
  return GetVertices(0.0, 0.0, 1.0, 1.0);
}

void Widget::SetPosition(double x, double y) noexcept {
  x_ = x;
  y_ = y;
}

void Widget::SetScale(double scale) noexcept { scale_ = scale; }

void Widget::SetProportion(double x_proportion, double y_proportion) noexcept {
  aspect_ = y_proportion / x_proportion;
}

void Widget::InsertChildWidget(std::unique_ptr<Widget> widget) {
  child_widgets_.push_back(std::move(widget));
}

std::vector<UIVertexFormat> Widget::GetVertices(double x, double y,
                                                double parent_aspect,
                                                double parent_scale) const {
  // Calculate aspect ratios sizes

  double aspect_scale =
      std::min(aspect_ / parent_aspect, parent_aspect / aspect_);

  double x_offset = x * parent_scale;
  double y_offset = y * parent_scale * parent_aspect;
  double new_scale = aspect_scale * parent_scale * scale_;

  double x_new = x + x_offset;
  double y_new = x + x_offset;

  double x_vert_offset = parent_scale * scale_;
  double y_vert_offset = new_scale;

  UIVertexFormat v0, v1, v2, v3;
  v0.x_ = x_new - x_vert_offset;
  v1.x_ = x_new + x_vert_offset;
  v2.x_ = x_new - x_vert_offset;
  v3.x_ = x_new + x_vert_offset;

  v0.y_ = y_new - y_vert_offset;
  v1.y_ = y_new - y_vert_offset;
  v2.y_ = y_new + y_vert_offset;
  v3.y_ = y_new + y_vert_offset;

  v0.use_texture_ = use_texture_;
  v1.use_texture_ = use_texture_;
  v2.use_texture_ = use_texture_;
  v3.use_texture_ = use_texture_;

  v0.uv_x_ = uv_x0_;
  v0.uv_y_ = uv_y0_;
  v1.uv_x_ = uv_x1_;
  v1.uv_y_ = uv_y0_;
  v2.uv_x_ = uv_x1_;
  v2.uv_y_ = uv_y1_;
  v3.uv_x_ = uv_x0_;
  v3.uv_y_ = uv_y1_;

  v0.r_ = r_;
  v0.g_ = g_;
  v0.b_ = b_;
  v0.a_ = a_;

  v1.r_ = r_;
  v1.g_ = g_;
  v1.b_ = b_;
  v1.a_ = a_;

  v2.r_ = r_;
  v2.g_ = g_;
  v2.b_ = b_;
  v2.a_ = a_;

  v3.r_ = r_;
  v3.g_ = g_;
  v3.b_ = b_;
  v3.a_ = a_;

  std::vector<UIVertexFormat> vertices = {v0, v1, v2, v3};

  for (const auto& widget : child_widgets_) {
    const auto& child_vertices =
        widget->GetVertices(x_new, y_new, aspect_, new_scale);
    vertices.insert(vertices.end(), child_vertices.begin(),
                    child_vertices.end());
  }

  return vertices;
}
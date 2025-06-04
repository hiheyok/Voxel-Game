// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>
#include <vector>

class UserInputs;

struct UIVertexFormat;

class Widget {
 public:
  Widget();
  virtual ~Widget();
  Widget(const Widget&) = delete;
  Widget& operator=(const Widget&) = delete;
  Widget(Widget&&) noexcept;
  Widget& operator=(Widget&&) noexcept;

  std::vector<UIVertexFormat> GetVertices() const;
  void SetPosition(double x, double y) noexcept;
  void SetScale(double scale) noexcept;
  void SetProportion(double x_proportion, double y_proportion) noexcept;
  void InsertChildWidget(std::unique_ptr<Widget> widget);
  virtual void Update(const UserInputs&) = 0;

 protected:
  // Input is the parent values
  virtual std::vector<UIVertexFormat> GetVertices(double x, double y,
                                                  double parent_aspect,
                                                  double parent_scale) const;

  // Using the proportions, it will first scale up until it fits within the
  // proportions of the parent widget. Then, it will be scaled using the scale
  // factor

  double aspect_ = 1.0;       // This is the proportions x_size / y_size
  double scale_ = 1.0;        // This is the size relative to the parent widget
  double x_ = 0.0, y_ = 0.0;  // Relative to the parent widget

  // Widget properties
  int texture_id_ = 0;
  bool use_texture_ = false;
  float uv_x0_ = 0.0, uv_y0_ = 0.0;
  float uv_x1_ = 0.0, uv_y1_ = 0.0;
  float r_ = 1.0, g_ = 1.0, b_ = 1.0, a_ = 1.0;

 private:
  std::vector<std::unique_ptr<Widget>> child_widgets_;
};

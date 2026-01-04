#include "RenderEngine/UI/Data/UserInterfaceData.h"

#include <cstdint>
#include <iostream>
#include <vector>

std::ostream& operator<<(std::ostream& s, const UIVertexFormat& obj) {
  s << "(" << obj.x_ << ',' << obj.y_ << ')';
  return s;
}

void UIRectangle::GetVertices(std::vector<UIVertexFormat>& vertices,
                              std::vector<uint32_t>& indices) const {
  uint32_t index_offset = vertices.size();

  UIVertexFormat v_base;
  v_base.r_ = rgba_.r;
  v_base.g_ = rgba_.g;
  v_base.b_ = rgba_.b;
  v_base.a_ = rgba_.a;

  v_base.tex_idx_ = tex_idx_;

  UIVertexFormat v00 = v_base, v01 = v_base, v10 = v_base, v11 = v_base;
  v00.x_ = pos_.x;
  v00.y_ = pos_.y;
  v01.x_ = pos_.x + size_.x;
  v01.y_ = pos_.y;
  v10.x_ = pos_.x;
  v10.y_ = pos_.y + size_.y;
  v11.x_ = pos_.x + size_.x;
  v11.y_ = pos_.y + size_.y;

  v00.u_ = uv_beg_.x;
  v00.v_ = uv_beg_.y;
  v01.u_ = uv_end_.x;
  v01.v_ = uv_beg_.y;
  v10.u_ = uv_beg_.x;
  v10.v_ = uv_end_.y;
  v11.u_ = uv_end_.x;
  v11.v_ = uv_end_.y;

  vertices.insert(vertices.end(), {v00, v01, v10, v11});
  indices.insert(indices.end(),
                 {index_offset + 0, index_offset + 1, index_offset + 2,
                  index_offset + 2, index_offset + 1, index_offset + 3});
}

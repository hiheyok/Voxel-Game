// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Render/RenderDrawElements.h"

#include "RenderEngine/OpenGL/Buffers/VertexArray.h"

RenderDrawElements::RenderDrawElements(GameContext& game_context,
                                       uint32_t draw_type)
    : RenderObject{game_context}, vbo_{game_context}, ebo_{game_context} {
  draw_type_ = draw_type;
  vbo_.SetType(GL_ARRAY_BUFFER);
  vbo_.SetUsage(GL_STATIC_DRAW);
  ebo_.SetType(GL_ELEMENT_ARRAY_BUFFER);
  ebo_.SetUsage(GL_STATIC_DRAW);
}

RenderDrawElements::~RenderDrawElements() = default;

void RenderDrawElements::Render() {
  vao_.Bind();
  shader_->Use();
  SetupTexture();
  glDrawElements(draw_type_, indices_count_, indices_type_,
                 reinterpret_cast<void*>(indices_offset_));
  vao_.Unbind();
}

void RenderDrawElements::SetDataAttribute(int index, int size, int type,
                                          int stride, int offset) {
  vao_.EnableAttriPtr(&vbo_, index, size, type, GL_FALSE, stride, offset);
}

void RenderDrawElements::SetIndicesCount(size_t count) noexcept {
  indices_count_ = count;
}
void RenderDrawElements::SetIndicesOffset(size_t offset) noexcept {
  indices_offset_ = offset;
}

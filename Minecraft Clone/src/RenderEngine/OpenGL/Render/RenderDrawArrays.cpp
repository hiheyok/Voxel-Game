// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Render/RenderDrawArrays.h"

#include "RenderEngine/OpenGL/Buffers/VertexArray.h"

RenderDrawArrays::RenderDrawArrays(GameContext& game_context,
                                   uint32_t draw_type)
    : RenderObject{game_context}, vbo_{game_context} {
  draw_type_ = draw_type;
  vbo_.SetType(GL_ARRAY_BUFFER);
  vbo_.SetUsage(GL_STATIC_DRAW);
}

RenderDrawArrays::~RenderDrawArrays() = default;

void RenderDrawArrays::Render() {
  vao_.Bind();
  shader_->Use();
  SetupTexture();
  glDrawArrays(draw_type_, indices_offset_, indices_count_);
  vao_.Unbind();
}

void RenderDrawArrays::SetDataAttribute(int index, int size, int type,
                                        int stride, int offset) {
  vao_.EnableAttriPtr(&vbo_, index, size, type, GL_FALSE, stride, offset);
}

void RenderDrawArrays::SetIndicesCount(size_t count) noexcept {
  indices_count_ = count;
}
void RenderDrawArrays::SetIndicesOffset(size_t offset) noexcept {
  indices_offset_ = offset;
}
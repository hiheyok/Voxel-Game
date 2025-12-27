// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Render/RenderDrawElementsInstanced.h"

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "Utils/LogUtils.h"

RenderDrawElementsInstanced::RenderDrawElementsInstanced(GameContext& context,
                                                         uint32_t draw_type)
    : RenderObject{context}, vbo_{context}, ebo_{context} {
  draw_type_ = draw_type;
  vbo_.SetType(GL_ARRAY_BUFFER);
  vbo_.SetUsage(GL_STATIC_DRAW);
  ebo_.SetType(GL_ELEMENT_ARRAY_BUFFER);
  ebo_.SetUsage(GL_STATIC_DRAW);
}

RenderDrawElementsInstanced::~RenderDrawElementsInstanced() = default;

void RenderDrawElementsInstanced::Render() {
  vao_.Bind();
  shader_->Use();
  SetupTexture();
  glDrawElementsInstanced(draw_type_, indices_count_, indices_type_,
                          reinterpret_cast<void*>(indices_offset_),
                          instance_count_);
  vao_.Unbind();
}

void RenderDrawElementsInstanced::SetDataAttribute(int index, int size,
                                                   int type, int stride,
                                                   int offset) {
  vao_.EnableAttriPtr(&vbo_, index, size, type, GL_FALSE, stride, offset);
}

void RenderDrawElementsInstanced::SetIndicesCount(size_t count) noexcept {
  indices_count_ = count;
}
void RenderDrawElementsInstanced::SetIndicesOffset(size_t offset) noexcept {
  indices_offset_ = offset;
}
void RenderDrawElementsInstanced::SetInstancesCount(size_t count) noexcept {
  instance_count_ = count;
}

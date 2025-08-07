// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Buffers/VertexArray.h"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/BufferStorage.h"
#include "Utils/LogUtils.h"

VertexArray::VertexArray(GameContext& context) : context_{context} {
  glGenVertexArrays(1, &array_id_);
  context_.logger_->LogDebug("VertexArray::GenArray",
                             "Created array. ID: " + std::to_string(array_id_));
}

VertexArray::~VertexArray() {
  if (array_id_ != 0) {
    glDeleteVertexArrays(1, &array_id_);
  }
}

VertexArray::VertexArray(VertexArray&& buffer) noexcept
    : context_{buffer.context_} {
  (*this) = std::move(buffer);
}

VertexArray& VertexArray::operator=(VertexArray&& buffer) noexcept {
  array_id_ = buffer.array_id_;
  buffer.array_id_ = 0;
  return *this;
}

void VertexArray::Bind() { glBindVertexArray(array_id_); }

void VertexArray::Unbind() { glBindVertexArray(0); }

VertexArray& VertexArray::EnableAttriPtr(Buffer* buffer, GLuint index,
                                         GLint size, GLenum type,
                                         GLboolean normalized, GLsizei stride,
                                         int sub_index) {
  Bind();
  buffer->Bind();
  glVertexAttribPointer(
      index, size, type, normalized, GetDataTypeSize(type) * stride,
      reinterpret_cast<void*>(GetDataTypeSize(type) * sub_index));
  glEnableVertexAttribArray(index);
  buffer->Unbind();
  Unbind();
  return *this;
}

VertexArray& VertexArray::EnableAttriPtr(BufferStorage* buffer, GLuint index,
                                         GLint size, GLenum type,
                                         GLboolean normalized, GLsizei stride,
                                         int sub_index) {
  Bind();
  buffer->Bind();
  glVertexAttribPointer(
      index, size, type, normalized, GetDataTypeSize(type) * stride,
      reinterpret_cast<void*>(GetDataTypeSize(type) * sub_index));
  glEnableVertexAttribArray(index);
  buffer->Unbind();
  Unbind();
  return *this;
}

void VertexArray::ResetArray() {
  glDeleteVertexArrays(1, &array_id_);
  glGenVertexArrays(1, &array_id_);
}

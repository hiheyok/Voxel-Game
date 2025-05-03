// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Buffers/VertexArray.h"

#include <GLFW/glfw3.h>

#include <utility>

#include "Utils/LogUtils.h"

VertexArray::VertexArray() {
  glGenVertexArrays(1, &array_id_);
  g_logger.LogDebug("VertexArray::GenArray",
                    "Created array. ID: " + std::to_string(array_id_));
}

VertexArray::~VertexArray() {
  if (array_id_ != 0) {
    glDeleteVertexArrays(1, &array_id_);
  }
}

VertexArray::VertexArray(VertexArray&& buffer) noexcept {
  (*this) = std::move(buffer);
}

VertexArray& VertexArray::operator=(VertexArray&& buffer) noexcept {
  array_id_ = buffer.array_id_;
  buffer.array_id_ = 0;
  return *this;
}

void VertexArray::Bind() { glBindVertexArray(array_id_); }

void VertexArray::Unbind() { glBindVertexArray(0); }

VertexArray& VertexArray::EnableAttriPTR(GLuint index, GLint size, GLenum type,
                                         GLboolean normalized, GLsizei stride,
                                         int subIndex) {
  glVertexAttribPointer(index, size, type, normalized,
                        sizeof(GL_FLOAT) * stride,
                        reinterpret_cast<void*>(subIndex * sizeof(uint32_t)));
  glEnableVertexAttribArray(index);
  return *this;
}

void VertexArray::ResetArray() {
  glDeleteVertexArrays(1, &array_id_);
  glGenVertexArrays(1, &array_id_);
}

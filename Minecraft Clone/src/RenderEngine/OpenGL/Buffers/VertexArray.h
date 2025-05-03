// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <gl/glew.h>
#include <cstdint>

class VertexArray {
 public:
  VertexArray();
  ~VertexArray();

  VertexArray(const VertexArray&) = delete;
  VertexArray& operator=(const VertexArray&) = delete;

  VertexArray(VertexArray&&) noexcept;
  VertexArray& operator=(VertexArray&&) noexcept;

  void Bind();
  void Unbind();
  void ResetArray();
  VertexArray& EnableAttriPTR(GLuint index, GLint size, GLenum type,
                              GLboolean normalized, GLsizei stride,
                              int subIndex);

 private:
  uint32_t array_id_ = 0;
};

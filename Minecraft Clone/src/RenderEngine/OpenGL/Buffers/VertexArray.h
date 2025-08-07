// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <gl/glew.h>

#include <cstdint>
#include <stdexcept>

class Buffer;
class BufferStorage;
class GameContext;

class VertexArray {
 public:
  explicit VertexArray(GameContext&);
  ~VertexArray();

  VertexArray(const VertexArray&) = delete;
  VertexArray& operator=(const VertexArray&) = delete;

  VertexArray(VertexArray&&) noexcept;
  VertexArray& operator=(VertexArray&&) noexcept;

  void Bind();
  void Unbind();
  void ResetArray();
  VertexArray& EnableAttriPtr(Buffer* buffer, GLuint index, GLint size,
                              GLenum type, GLboolean normalized, GLsizei stride,
                              int sub_index);

  VertexArray& EnableAttriPtr(BufferStorage* buffer, GLuint index, GLint size,
                              GLenum type, GLboolean normalized, GLsizei stride,
                              int sub_index);

 private:
  static constexpr size_t GetDataTypeSize(uint32_t data_type) {
    switch (data_type) {
      case GL_BYTE:
        return 1;
      case GL_UNSIGNED_BYTE:
        return 1;
      case GL_SHORT:
        return 2;
      case GL_UNSIGNED_SHORT:
        return 2;
      case GL_INT:
        return 4;
      case GL_UNSIGNED_INT:
        return 4;
      case GL_FIXED:
        return 4;
      case GL_HALF_FLOAT:
        return 2;
      case GL_FLOAT:
        return 4;
      case GL_DOUBLE:
        return 8;
      default:
        throw std::runtime_error("Invalid datatype");
    }
  }

  GameContext& context_;
  uint32_t array_id_ = 0;
};

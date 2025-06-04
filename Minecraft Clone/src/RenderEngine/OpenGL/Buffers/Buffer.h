// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <gl/glew.h>

#include <cstdint>
#include <vector>

class GameContext;

class Buffer {
 public:
  explicit Buffer(GameContext&);
  ~Buffer();

  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

  Buffer(Buffer&&) noexcept;
  Buffer& operator=(Buffer&&) noexcept;

  void SetType(GLenum type);
  void SetMaxSize(size_t maxSize);
  void SetUsage(GLenum drawType);

  void ResetBuffer();

  void InitializeData();

  void BindBase(int index);

  void UnbindBase(int index);

  void Bind() const;

  void Unbind() const;

  void GetData(uint32_t* ptr, size_t offset, size_t size);

  void CopyFrom(const Buffer& buffer, size_t readOffset, size_t writeOffset,
                size_t size);
  void CopyTo(const Buffer& destination, size_t offset, size_t desOffset,
              size_t size);

  [[deprecated]]
  void InsertData(GLsizeiptr size, const void* data, GLenum usage);

  template <typename T>
  void InsertData(const std::vector<T>& data, GLenum usage) {
    Bind();
    glBufferData(type_, data.size() * sizeof(T),
                 reinterpret_cast<const void*>(data.data()), usage);
    Unbind();
  }

  void InsertSubData(GLintptr Offset, GLsizeiptr Size, const void* Data);

  void SetStorage(int size);

  operator uint32_t() const noexcept;
  uint32_t GetId() const noexcept;

 private:
  GameContext& game_context_;
  size_t max_size_ = 0;
  uint32_t buffer_id_ = 0;
  uint32_t type_ = 0, usage_ = 0;
};

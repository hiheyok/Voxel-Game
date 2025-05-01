#pragma once
#include <cstdint>
#include <gl/glew.h>

class Buffer {
 public:
  Buffer();
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

  void Bind();

  void Unbind();

  void GetData(uint32_t* ptr, size_t offset, size_t size);

  void CopyFrom(Buffer buffer, size_t readOffset, size_t writeOffset,
                size_t size);

  void InsertData(GLsizeiptr size, const void* data, GLenum usage);
  void InsertSubData(GLintptr Offset, GLsizeiptr Size, const void* Data);

  void SetStorage(int size);

  void CopyTo(Buffer& destination, size_t offset, size_t desOffset,
              size_t size);

  operator uint32_t() const noexcept;
  uint32_t GetId() const noexcept;

 private:
  size_t max_size_ = 0;
  uint32_t buffer_id_ = 0;
  uint32_t type_ = 0, usage_ = 0;
};

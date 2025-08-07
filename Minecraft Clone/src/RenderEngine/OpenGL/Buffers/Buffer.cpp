// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Buffers/Buffer.h"

#include <stdexcept>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

Buffer::Buffer(GameContext& context) : context_{context} {
  glGenBuffers(1, &buffer_id_);
  if (buffer_id_ == 0) {
    throw std::runtime_error("Buffer::GenBuffer - glGenBuffers failed!");
  }
  context_.logger_->LogDebug(
      "Buffer::GenBuffer", "Created buffer. ID: " + std::to_string(buffer_id_));
}

Buffer::~Buffer() {
  if (buffer_id_ != 0) {
    glDeleteBuffers(1, &buffer_id_);
  }
}

Buffer::Buffer(Buffer&& buffer) noexcept : context_(buffer.context_) {
  (*this) = std::move(buffer);
}

Buffer& Buffer::operator=(Buffer&& buffer) noexcept {
  max_size_ = buffer.max_size_;
  buffer_id_ = buffer.buffer_id_;
  type_ = buffer.type_;
  usage_ = buffer.usage_;

  buffer.max_size_ = 0;
  buffer.buffer_id_ = 0;
  buffer.type_ = 0;
  buffer.usage_ = 0;
  return *this;
}

void Buffer::InsertData(GLsizeiptr size, const void* data, GLenum usage) {
  Bind();
  glBufferData(type_, size, data, usage);
  Unbind();
}

void Buffer::InsertSubData(GLintptr offset, GLsizeiptr size, const void* data) {
  Bind();
  glBufferSubData(type_, offset, size, data);
  Unbind();
}

void Buffer::ResetBuffer() { glBufferData(type_, 0, nullptr, usage_); }

void Buffer::Bind() const { glBindBuffer(type_, buffer_id_); }

void Buffer::Unbind() const { glBindBuffer(type_, 0); }

void Buffer::SetMaxSize(size_t maxSize) { max_size_ = maxSize; }

void Buffer::SetType(GLenum type) { type_ = type; }

void Buffer::SetUsage(GLenum usage) { usage_ = usage; }

void Buffer::InitializeData() {
  Bind();
  glBufferData(type_, max_size_, nullptr, usage_);
  Unbind();
}

void Buffer::BindBase(int index) { glBindBufferBase(type_, index, buffer_id_); }

void Buffer::UnbindBase(int index) { glBindBufferBase(type_, index, 0); }

void Buffer::GetData(uint32_t* ptr, size_t offset, size_t size) {
  Bind();
  glGetBufferSubData(type_, offset, size, static_cast<void*>(ptr));
  Unbind();
}

void Buffer::CopyFrom(const Buffer& buffer, size_t readOffset,
                      size_t writeOffset, size_t size) {
  glBindBuffer(GL_COPY_READ_BUFFER, buffer.buffer_id_);
  glBindBuffer(GL_COPY_WRITE_BUFFER, buffer_id_);
  glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset,
                      writeOffset, size);
  glBindBuffer(GL_COPY_READ_BUFFER, 0);
  glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void Buffer::CopyTo(const Buffer& destination, size_t read_offset,
                    size_t write_offset, size_t size) {
  glBindBuffer(GL_COPY_READ_BUFFER, buffer_id_);
  glBindBuffer(GL_COPY_WRITE_BUFFER, destination.buffer_id_);
  glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, read_offset,
                      write_offset, size);
  glBindBuffer(GL_COPY_READ_BUFFER, 0);
  glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

Buffer::operator uint32_t() const noexcept { return buffer_id_; }

uint32_t Buffer::GetId() const noexcept { return buffer_id_; }

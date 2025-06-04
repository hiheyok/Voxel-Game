// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Buffers/BufferStorage.h"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/OpenGL/Shader/ComputeShader.h"
#include "Utils/LogUtils.h"

BufferStorage::BufferStorage(GameContext& game_context, GLuint bufferTarget,
                             uint64_t size, bool dynamic, const void* data)
    : game_context_{game_context},
      copy_shader_{std::make_unique<ComputeShader>(
          game_context_, "assets/shaders/Buffers/CopyData.glsl")},
      move_shader_{std::make_unique<ComputeShader>(
          game_context_, "assets/shaders/Buffers/MoveData.glsl")} {
  target_ = bufferTarget;
  max_size_ = size;

  glGenBuffers(1, &buffer_storage_id_);

  if (buffer_storage_id_ == 0) {
    throw std::runtime_error("BufferStorage::Create - glGenBuffers failed!");
  }

  game_context_.logger_->LogDebug(
      "BufferStorage::Create",
      "Generated buffer storage. ID: " + std::to_string(buffer_storage_id_));
  Bind();

  // *** Use glBufferStorage ***
  GLbitfield flags = 0;
  if (dynamic) {
    flags |= GL_DYNAMIC_STORAGE_BIT;  // Crucial for
                                      // glBufferSubData/glCopyBufferSubData
  }
  // *** NO MAPPING FLAGS (GL_MAP_*) are included ***
  // This strongly hints the driver to keep the buffer in VRAM if possible.

  glBufferStorage(target_, static_cast<GLsizeiptr>(size), data, flags);

  // Check for OpenGL errors after buffer storage creation
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    throw std::runtime_error(
        "BufferStorage::Create - " +
        std::string(
            "OpenGL error after glBufferStorage: "));  // std::string(err)
  }

  game_context_.logger_->LogInfo(
      "BufferStorage::Create",
      "Created buffer storage ID: " + std::to_string(buffer_storage_id_) +
          " Size: " + std::to_string(size) + " bytes, Target: " +
          std::to_string(target_) + ", Dynamic: " + (dynamic ? "Yes" : "No"));
}

BufferStorage::~BufferStorage() {
  if (buffer_storage_id_ != 0) {
    glDeleteBuffers(1, &buffer_storage_id_);
    buffer_storage_id_ = 0;  // Use 0 to indicate deleted/uninitialized
    max_size_ = 0;
    target_ = 0;
    game_context_.logger_->LogDebug(
        "BufferStorage::Delete",
        "Deleted buffer storage. ID was: " +
            std::to_string(buffer_storage_id_));  // Log before setting to 0
  }
}

BufferStorage::BufferStorage(BufferStorage&& buffer) noexcept
    : game_context_{buffer.game_context_} {
  (*this) = std::move(buffer);
}

BufferStorage& BufferStorage::operator=(BufferStorage&& buffer) noexcept {
  buffer_storage_id_ = buffer.buffer_storage_id_;
  max_size_ = buffer.max_size_;
  target_ = buffer.target_;
  copy_shader_ = std::move(buffer.copy_shader_);
  move_shader_ = std::move(buffer.move_shader_);

  buffer.buffer_storage_id_ = 0;
  buffer.max_size_ = 0;
  buffer.target_ = 0;
  return *this;
}

void BufferStorage::Bind() {
  if (buffer_storage_id_ != 0) {
    glBindBuffer(target_, buffer_storage_id_);
  } else {
    game_context_.logger_->LogWarn(
        "BufferStorage::Bind",
        "Attempted to bind an uninitialized/deleted buffer storage.");
  }
}

void BufferStorage::Unbind() { glBindBuffer(target_, 0); }

void BufferStorage::InsertData(uint64_t offset, uint64_t size,
                               const void* data) {
  if (buffer_storage_id_ == 0) {
    throw std::logic_error(
        "BufferStorage::InsertData - Attempted InsertData on uninitialized "
        "buffer.");
  }

  if (offset + size > max_size_) {
    throw std::logic_error(
        "BufferStorage::InsertData - " +
        std::string("InsertData exceeds buffer bounds. Offset: ") +
        std::to_string(offset) + ", Size: " + std::to_string(size) +
        ", MaxSize: " + std::to_string(max_size_));
  }

  Bind();
  glBufferSubData(target_, offset, size, data);
}

void BufferStorage::CopyFrom(BufferStorage& sourceBuffer, size_t readOffset,
                             size_t writeOffset, size_t size) {
  CopyFrom(&sourceBuffer, readOffset, writeOffset, size);
}

void BufferStorage::CopyTo(BufferStorage& destinationBuffer, size_t readOffset,
                           size_t writeOffset, size_t size) {
  CopyTo(&destinationBuffer, readOffset, writeOffset, size);
}

void BufferStorage::CopyFrom(BufferStorage* sourceBuffer, size_t readOffset,
                             size_t writeOffset, size_t size) {
  if (buffer_storage_id_ == 0 || sourceBuffer->buffer_storage_id_ == 0) {
    throw std::logic_error(
        "BufferStorage::CopyFrom - Attempted CopyFrom with uninitialized "
        "buffer(s).");
  }
  if (writeOffset + size > max_size_ ||
      readOffset + size > sourceBuffer->max_size_) {
    throw std::range_error(
        "BufferStorage::CopyFrom - CopyFrom exceeds buffer bounds.");
    return;
  }

  copy_shader_->BindBufferAsSSBO(sourceBuffer->buffer_storage_id_, 0)
      .BindBufferAsSSBO(buffer_storage_id_, 1)
      .SetInt("srcOffset", readOffset / sizeof(uint32_t))
      .SetInt("dstOffset", writeOffset / sizeof(uint32_t))
      .SetInt("copySize", size / sizeof(uint32_t));

  copy_shader_->DispatchCompute(
      static_cast<int>(
          ceil(static_cast<double>(size) / kWorkGroupSize / sizeof(uint32_t))),
      1, 1);
  copy_shader_->SSBOMemoryBarrier();
}

void BufferStorage::CopyTo(BufferStorage* destinationBuffer, size_t readOffset,
                           size_t writeOffset, size_t size) {
  if (buffer_storage_id_ == 0 || destinationBuffer->buffer_storage_id_ == 0) {
    game_context_.logger_->LogError(
        "BufferStorage::CopyTo",
        "Attempted CopyTo with uninitialized buffer(s).");
    return;
  }
  if (readOffset + size > max_size_ ||
      writeOffset + size > destinationBuffer->max_size_) {
    throw std::range_error(
        "BufferStorage::CopyTo - CopyTo exceeds buffer bounds.");
  }
  copy_shader_->BindBufferAsSSBO(buffer_storage_id_, 0)
      .BindBufferAsSSBO(destinationBuffer->buffer_storage_id_, 1)
      .SetInt("srcOffset", readOffset / sizeof(uint32_t))
      .SetInt("dstOffset", writeOffset / sizeof(uint32_t))
      .SetInt("copySize", size / sizeof(uint32_t));

  copy_shader_->DispatchCompute(
      static_cast<int>(
          ceil(static_cast<double>(size) / kWorkGroupSize / sizeof(uint32_t))),
      1, 1);
  copy_shader_->SSBOMemoryBarrier();
}

void BufferStorage::MoveData(size_t readOffset, size_t writeOffset,
                             size_t size) {
  if (buffer_storage_id_ == 0) {
    throw std::logic_error(
        "BufferStorage::MoveData - Attempted MoveData with uninitialized "
        "buffer(s).");
  }
  if (readOffset + size > max_size_ || writeOffset + size > max_size_) {
    throw std::range_error(
        "BufferStorage::MoveData - MoveData exceeds buffer bounds.");
  }

  move_shader_->BindBufferAsSSBO(buffer_storage_id_, 0)
      .SetInt("srcOffset", readOffset / sizeof(uint32_t))
      .SetInt("dstOffset", writeOffset / sizeof(uint32_t))
      .SetInt("copySize", size / sizeof(uint32_t));

  move_shader_->DispatchCompute(
      static_cast<int>(
          ceil(static_cast<double>(size) / kWorkGroupSize / sizeof(uint32_t))),
      1, 1);
  move_shader_->SSBOMemoryBarrier();
}

bool BufferStorage::IsInitialized() const { return buffer_storage_id_ != 0; }

size_t BufferStorage::GetMaxSize() const { return max_size_; }

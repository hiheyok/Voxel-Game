// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/Batch/ChunkBatch.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <utility>
#include <vector>

#include "RenderEngine/ChunkRender/Mesh/BlockVertexFormat.h"
#include "RenderEngine/OpenGL/Buffers/BufferStorage.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "Utils/Timer/Timer.h"

ChunkDrawBatch::ChunkDrawBatch(GameContext& game_context, size_t maxSize)
    : game_context_{game_context},
      memory_pool_{game_context, maxSize},
      ibo_{game_context},
      ssbo_{game_context},
      array_{game_context},
      max_buffer_size_{maxSize} {}

ChunkDrawBatch::ChunkDrawBatch(ChunkDrawBatch&&) = default;
ChunkDrawBatch::~ChunkDrawBatch() = default;

void ChunkDrawBatch::SetupBuffers() {
  ibo_.SetType(GL_DRAW_INDIRECT_BUFFER);
  ssbo_.SetType(GL_SHADER_STORAGE_BUFFER);

  ibo_.SetUsage(GL_STATIC_DRAW);
  ssbo_.SetUsage(GL_DYNAMIC_COPY);

  ibo_.SetMaxSize(max_buffer_size_ / 100);
  ibo_.InitializeData();

  array_
      .EnableAttriPtr(memory_pool_.buffer_.get(), 0, 3, GL_FLOAT, GL_FALSE, 5,
                      0)
      .EnableAttriPtr(memory_pool_.buffer_.get(), 1, 1, GL_FLOAT,
                      GL_FALSE, 5, 3)
      .EnableAttriPtr(memory_pool_.buffer_.get(), 2, 1, GL_FLOAT,
                      GL_FALSE, 5, 4);

  ssbo_.SetMaxSize(max_buffer_size_ / 100);
  ssbo_.InitializeData();
}

void ChunkDrawBatch::Reset() {
  ibo_.ResetBuffer();
  ssbo_.ResetBuffer();
  array_.ResetArray();
  SetupBuffers();
}

void ChunkDrawBatch::GenDrawCommands(int renderDistance,
                                     int verticalRenderDistance) {
  Timer time;

  if (render_list_.size() > draw_commands_.size()) {
    UpdateCommandBufferSize();
  }

  if (!update_commands_) return;

  frustum_.CalculateFrustum(camera_);

  glm::ivec3 position(floor(camera_->position_.x / 16.f),
                      floor(camera_->position_.y / 16.f),
                      floor(camera_->position_.z / 16.f));

  int index = 1;

  for (const auto& data : render_list_arr_) {
    int x = data.position_.x;
    int y = data.position_.y;
    int z = data.position_.z;

    float deltaX = static_cast<float>((x - position.x) * (x - position.x));
    float deltaY = static_cast<float>((y - position.y) * (y - position.y));
    float deltaZ = static_cast<float>((z - position.z) * (z - position.z));

    float dx2 = deltaX / (renderDistance * renderDistance);
    float dy2 = deltaY / (verticalRenderDistance * verticalRenderDistance);
    float dz2 = deltaZ / (renderDistance * renderDistance);

    if (dx2 + dy2 + dz2 < 1.f) {
      if (frustum_.SphereInFrustum(static_cast<float>(x << kChunkDimLog2),
                                   static_cast<float>(y << kChunkDimLog2),
                                   static_cast<float>(z << kChunkDimLog2),
                                   24.3f)) {
        draw_commands_[index - 1].set(
            static_cast<uint32_t>(data.mem_size_ / sizeof(BlockVertexFormat)), 1,
            static_cast<uint32_t>(data.mem_offset_ / sizeof(BlockVertexFormat)), index);
        chunk_shader_pos_[(index - 1) * 3 + 0] = x;
        chunk_shader_pos_[(index - 1) * 3 + 1] = y;
        chunk_shader_pos_[(index - 1) * 3 + 2] = z;

        index++;
      }
    }
  }

  index--;

  amount_of_chunks_being_rendered_ = index;

  ssbo_.InsertSubData(0, (amount_of_chunks_being_rendered_ * 3) * sizeof(int),
                      chunk_shader_pos_.data());
  ibo_.InsertSubData(
      0, amount_of_chunks_being_rendered_ * sizeof(DrawCommandIndirect),
      draw_commands_.data());
}

void ChunkDrawBatch::UpdateCommandBufferSize() {
  draw_commands_.resize(render_list_.size());
  chunk_shader_pos_.resize(render_list_.size() * 3);
}

bool ChunkDrawBatch::AddChunkVertices(
    const std::vector<BlockVertexFormat>& data, ChunkPos pos) {
  ChunkMemoryPoolOffset memoryPoolBlockData = memory_pool_.AddChunk(data, pos);

  if (memoryPoolBlockData.mem_offset_ == ULLONG_MAX) {
    return false;
  }

  if (render_list_.contains(memoryPoolBlockData.mem_offset_)) {
    size_t idx = render_list_[memoryPoolBlockData.mem_offset_];
    render_list_arr_[idx] = memoryPoolBlockData;
  } else {
    render_list_[memoryPoolBlockData.mem_offset_] = render_list_arr_.size();
    render_list_arr_.emplace_back(memoryPoolBlockData);
  }

  update_commands_ = true;
  return true;
}

void ChunkDrawBatch::DeleteChunkVertices(ChunkPos pos) {
  if (memory_pool_.CheckChunk(pos)) {
    ChunkMemoryPoolOffset ChunkMemOffset =
        memory_pool_.GetChunkMemoryPoolOffset(pos);
    if (ChunkMemOffset.mem_offset_ == std::numeric_limits<size_t>::max()) {
      throw std::logic_error("ChunkDrawBatch::DeleteChunkVertices - " +
                             std::string("Failed to delete chunk: ") +
                             std::to_string(pos));
    }

    size_t idx = render_list_[ChunkMemOffset.mem_offset_];
    render_list_[render_list_arr_.back().mem_offset_] = idx;
    std::swap(render_list_arr_.back(), render_list_arr_[idx]);
    render_list_arr_.pop_back();
    render_list_.erase(ChunkMemOffset.mem_offset_);

    memory_pool_.DeleteChunk(pos);
    update_commands_ = true;
  }
}

void ChunkDrawBatch::Bind() {
  array_.Bind();
  ibo_.Bind();
  memory_pool_.buffer_->Bind();
  ssbo_.Bind();
  ssbo_.BindBase(2);
}

void ChunkDrawBatch::Unbind() {
  ssbo_.UnbindBase(2);
  ssbo_.Unbind();
  ibo_.Unbind();
  memory_pool_.buffer_->Unbind();
  array_.Unbind();
}

void ChunkDrawBatch::Draw(Shader* shader) {
  Bind();
  shader->Use();
  glMultiDrawArraysIndirect(
      GL_TRIANGLES, reinterpret_cast<void*>(0),
      static_cast<GLsizei>(amount_of_chunks_being_rendered_), 0);
  Unbind();
}

void ChunkDrawBatch::Defrag(size_t iterations) {
  size_t fragmentCount = memory_pool_.memory_pool_.GetFreeSpaceFragmentCount();
  if (fragmentCount == 1) {
    return;
  }

  iterations = std::min(iterations, fragmentCount - 1);

  for (size_t i = 0; i < iterations &&
                     memory_pool_.memory_pool_.free_memory_blocks_.size() != 1;
       ++i) {
    MemoryManagement::MemoryBlock freeMemoryBlock =
        memory_pool_.memory_pool_.free_memory_blocks_.begin()->second;
    size_t freeSpaceOffset = freeMemoryBlock.offset_;

    auto reserve =
        memory_pool_.memory_pool_.reserved_memory_blocks_.GetIterator(
            freeMemoryBlock.size_ + freeMemoryBlock.offset_);
    MemoryManagement::MemoryBlock reservedBlock = reserve->second;

    ChunkPos pos = memory_pool_.memory_chunk_offsets_[reservedBlock.offset_];
    DeleteChunkVertices(pos);

    ChunkMemoryPoolOffset memoryPoolBlockData;
    if (abs(static_cast<int64_t>(freeMemoryBlock.offset_) -
            static_cast<int64_t>(reservedBlock.offset_)) <=
        static_cast<int64_t>(reservedBlock.size_)) {  // If it overlap, use
                                                      // the stagging buffer
      memory_pool_.buffer_->CopyTo(memory_pool_.stagging_buffer_.get(),
                                   reservedBlock.offset_, 0,
                                   reservedBlock.size_);
      memoryPoolBlockData = memory_pool_.AddChunkStaggingBuffer(
          pos, freeSpaceOffset, reservedBlock.size_);
    } else {
      memory_pool_.buffer_->MoveData(
          reservedBlock.offset_, freeMemoryBlock.offset_, reservedBlock.size_);
      memoryPoolBlockData =
          memory_pool_.AddChunkMove(pos, freeSpaceOffset, reservedBlock.size_);
    }

    render_list_[memoryPoolBlockData.mem_offset_] = render_list_arr_.size();
    render_list_arr_.emplace_back(memoryPoolBlockData);

    update_commands_ = true;
  }
}

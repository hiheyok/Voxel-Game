// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <gl/glew.h>

#include <array>
#include <list>
#include <map>
#include <vector>

#include "Level/Chunk/Chunk.h"
#include "RenderEngine/Camera/camera.h"
#include "RenderEngine/ChunkRender/Batch/RenderCommandBuffer.h"
#include "RenderEngine/ChunkRender/MemoryPool/ChunkMemoryPool.h"
#include "RenderEngine/Frustum/frustum.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "Utils/LogUtils.h"
#include "Utils/MathHelper.h"

class Shader;
class GameContext;

class ChunkDrawBatch {
 public:
  ChunkDrawBatch(GameContext&, size_t maxSize);
  ChunkDrawBatch(const ChunkDrawBatch&) = delete;
  ChunkDrawBatch(ChunkDrawBatch&&);
  ~ChunkDrawBatch();

  void SetupBuffers();
  void Reset();
  void GenDrawCommands(int RenderDistance, int verticalRenderDistance);
  bool AddChunkVertices(const std::vector<uint32_t>& Data, ChunkPos pos);
  void DeleteChunkVertices(ChunkPos ID);
  void Draw(Shader* shader);
  void Defrag(size_t iterations);
  void UpdateCommandBufferSize();
  
  GameContext& game_context_;
  Camera* camera_ = nullptr;
  FastHashMap<size_t, size_t> render_list_;
  double debug_time_ = 0.0;
  ChunkGPUMemoryPool memory_pool_;

 private:
  void Bind();
  void Unbind();

  std::vector<ChunkMemoryPoolOffset> render_list_arr_;
  CFrustum frustum_;
  Buffer ibo_, ssbo_;
  VertexArray array_;
  size_t max_buffer_size_ = 0;
  std::vector<GLint> chunk_shader_pos_;
  std::vector<DrawCommandIndirect> draw_commands_;
  size_t amount_of_chunks_being_rendered_ = 0;
  bool update_commands_ = false;
};

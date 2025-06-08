// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Render/WorldRender.h"

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "Client/ClientLevel/ClientCache.h"
#include "Client/Player/PlayerPOV.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Options/Option.h"
#include "RenderEngine/ChunkRender/Mesh/BlockVertexFormat.h"
#include "RenderEngine/ChunkRender/Mesh/ChunkMesh.h"
#include "RenderEngine/ChunkRender/TerrainRenderer.h"
#include "Utils/Clock.h"
#include "Utils/Timer/Timer.h"

WorldRender::WorldRender(GameContext& game_context, PlayerPOV* player)
    : game_context_{game_context},
      renderer_{std::make_unique<TerrainRenderer>(game_context)},
      player_{player} {}

WorldRender::~WorldRender() = default;

void WorldRender::Render() {
  renderer_->RenderSky();
  renderer_->Render();
}

void WorldRender::LoadChunkToRenderer(ChunkPos chunk) {
  if (cache_->GetChunk(chunk) != nullptr) {
    mesh_thread_pool_->SubmitTask(chunk);
  }
}

void WorldRender::LoadChunkMultiToRenderer(std::vector<ChunkPos> chunks) {
  mesh_thread_pool_->SubmitTask(chunks);
}

std::unique_ptr<Mesh::ChunkVertexData> WorldRender::Worker(ChunkPos pos) {
  Chunk* chunk = WorldRender::cache_->GetChunk(pos);
  Mesh::ChunkMeshData& chunk_mesher = GetMesher();
  Timer timer;
  chunk_mesher.use_option_ = false;


  chunk_mesher.Reset();
  chunk_mesher.SetChunk(chunk);
  chunk_mesher.GenerateMesh();
  size_t time = static_cast<size_t>(timer.GetTimePassed_μs());

  // Transfer Infomation
  std::unique_ptr<Mesh::ChunkVertexData> data =
      std::make_unique<Mesh::ChunkVertexData>();

  data->solid_vertices_.insert(data->solid_vertices_.end(),
                               chunk_mesher.vertices_buffer_.begin(),
                               chunk_mesher.vertices_buffer_.begin() +
                                   chunk_mesher.solid_face_count_ * 6);
  data->transparent_vertices_.insert(
      data->transparent_vertices_.end(),
      chunk_mesher.transparent_vertices_buffer_.begin(),
      chunk_mesher.transparent_vertices_buffer_.begin() +
          chunk_mesher.transparent_face_count_ * 6);

  data->position_ = pos;

  build_stage_0_ += chunk_mesher.greedy_time_;
  build_stage_1_ += chunk_mesher.cache_time_;
  build_time_ += time;

  ++amount_of_mesh_generated_;

  return data;
}

void WorldRender::Update(std::vector<ChunkPos> updatedChunks) {
  static constexpr int kChunkUpdateLimit = 4000;
  int updateAmount = 0;

  std::vector<std::unique_ptr<Mesh::ChunkVertexData>> output =
      mesh_thread_pool_->GetOutput();

  mesh_add_queue_.insert(mesh_add_queue_.end(),
                         std::make_move_iterator(output.begin()),
                         std::make_move_iterator(output.end()));

  while (!mesh_add_queue_.empty()) {
    if (kChunkUpdateLimit == updateAmount) {
      break;
    }
    renderer_->AddChunk(std::move(mesh_add_queue_.back()));
    mesh_add_queue_.pop_back();
    renderer_->Defrag(2);
  }

  LoadChunkMultiToRenderer(updatedChunks);

  renderer_->Defrag(1);

  renderer_->Update();
  renderer_->PrepareRenderer();
}

void WorldRender::Start(GLFWwindow* window, ClientCache* cache,
                        PerformanceProfiler* profiler) {
  horizontal_render_distance_ =
      game_context_.options_->horizontal_render_distance_;
  vertical_render_distance_ = game_context_.options_->vertical_render_distance_;

  size_t threadCount = game_context_.options_->mesh_threads_;

  mesh_thread_pool_ = std::make_unique<ThreadPool<ChunkPos, WorkerReturnType>>(
      threadCount, "Mesher", std::bind_front(&WorldRender::Worker, this), 250);

  window_ = window;
  WorldRender::cache_ = cache;

  renderer_->Initialize(window_, player_->GetCamera());
  renderer_->LoadAssets();
  renderer_->SetSettings(horizontal_render_distance_, vertical_render_distance_,
                         90);

  profiler_ = profiler;
}

size_t WorldRender::GetQueuedSize() const noexcept {
  return mesh_thread_pool_->GetQueueSize();
}

Mesh::ChunkMeshData& WorldRender::GetMesher() {
  thread_local Mesh::ChunkMeshData mesher{game_context_};
  return mesher;
}
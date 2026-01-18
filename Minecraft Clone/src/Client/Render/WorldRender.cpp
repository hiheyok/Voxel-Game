// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Render/WorldRender.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "Client/ClientLevel/ClientCache.h"
#include "Client/Player/PlayerPOV.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Options/Option.h"
#include "Core/Position/PositionTypes.h"
#include "RenderEngine/ChunkRender/Mesh/BlockVertexFormat.h"
#include "RenderEngine/ChunkRender/Mesh/ChunkMesh.h"
#include "RenderEngine/ChunkRender/TerrainRenderer.h"
#include "Utils/ThreadPool.h"
#include "Utils/Timer/Timer.h"

using std::make_move_iterator;
using std::make_unique;
using std::move;
using std::unique_ptr;
using std::vector;

WorldRender::WorldRender(GameContext& context, PlayerPOV* player)
    : context_{context},
      renderer_{make_unique<TerrainRenderer>(context)},
      player_{player} {}

WorldRender::~WorldRender() = default;

void WorldRender::Render() {
  renderer_->RenderSky();
  renderer_->Render();
}

void WorldRender::LoadChunkToRenderer(ChunkPos chunk) {
  if (cache_->CheckChunk(chunk)) {
    mesh_thread_pool_->SubmitTask(chunk);
  }
}

void WorldRender::LoadChunkMultiToRenderer(const vector<ChunkPos>& chunks) {
  mesh_thread_pool_->SubmitTask(chunks);
}

unique_ptr<Mesh::ChunkVertexData> WorldRender::Worker(ChunkPos pos,
                                                      int worker_id) {
  Chunk& chunk = cache_->GetChunk(pos);
  Mesh::ChunkMeshData& chunk_mesher = *meshers_[worker_id];
  Timer timer;
  chunk_mesher.use_option_ = false;

  chunk_mesher.Reset();
  chunk_mesher.SetChunk(&chunk);
  chunk_mesher.GenerateMesh();
  size_t time = static_cast<size_t>(timer.GetTimePassed_us());

  // Transfer Infomation
  // Checks if there are any available vertex objects
  unique_ptr<Mesh::ChunkVertexData> data;
  bool success = vertex_data_pool_.try_dequeue(data);
  if (!success) {
    data = make_unique<Mesh::ChunkVertexData>();
  }

  int solid_vert_count = chunk_mesher.solid_face_count_ * 6;
  int trans_vert_count = chunk_mesher.trans_face_count_ * 6;

  data->solid_vertices_.resize(solid_vert_count);
  data->transparent_vertices_.resize(trans_vert_count);

  memcpy(data->solid_vertices_.data(), chunk_mesher.solid_buffer_.data(),
         sizeof(BlockVertexFormat) * solid_vert_count);
  memcpy(data->transparent_vertices_.data(), chunk_mesher.trans_buffer_.data(),
         sizeof(BlockVertexFormat) * trans_vert_count);

  data->position_ = pos;

  build_stage_0_ += chunk_mesher.greedy_time_;
  build_stage_1_ += chunk_mesher.cache_time_;
  build_time_ += time;

  set_face_calls_ += chunk_mesher.add_face_call_count_;
  set_face_calls_clocks_ += chunk_mesher.add_face_clock_;

  ++amount_of_mesh_generated_;

  return data;
}

void WorldRender::Update() {
  static constexpr int kChunkUpdateLimit = 4096;
  int updateAmount = 0;
  static size_t frame_count_ = 0;

  vector<unique_ptr<Mesh::ChunkVertexData>> output =
      mesh_thread_pool_->GetOutput();

  mesh_add_queue_.insert(mesh_add_queue_.end(),
                         make_move_iterator(output.begin()),
                         make_move_iterator(output.end()));

  while (!mesh_add_queue_.empty()) {
    if (kChunkUpdateLimit == updateAmount) {
      break;
    }
    renderer_->AddChunk(*mesh_add_queue_.back());
    vertex_data_pool_.enqueue(move(mesh_add_queue_.back()));

    mesh_add_queue_.pop_back();
    updateAmount++;
  }

  if (frame_count_++ % 25 == 0) {
    renderer_->Defrag(4);
  }

  renderer_->Update();
  renderer_->PrepareRenderer();
}

void WorldRender::Start(GLFWwindow* window, ClientCache* cache) {
  horizontal_render_distance_ = context_.options_->horizontal_render_distance_;
  vertical_render_distance_ = context_.options_->vertical_render_distance_;

  size_t threadCount = context_.options_->mesh_threads_;

  for (size_t i = 0; i < threadCount; ++i) {
    meshers_.emplace_back(make_unique<Mesh::ChunkMeshData>(context_));
  }

  mesh_thread_pool_ = make_unique<ThreadPool<ChunkPos, WorkerReturnType>>(
      threadCount, "Mesher", std::bind_front(&WorldRender::Worker, this), 250);

  window_ = window;
  cache_ = cache;

  renderer_->Initialize(window_, player_->GetCamera());
  renderer_->LoadAssets();
  renderer_->SetSettings(horizontal_render_distance_, vertical_render_distance_,
                         90);
}

size_t WorldRender::GetQueuedSize() const noexcept {
  return mesh_thread_pool_->GetQueueSize();
}

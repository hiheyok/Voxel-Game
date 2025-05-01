#include "Client/Render/WorldRender.h"

#include <functional>

#include "Client/ClientLevel/ClientCache.h"
#include "Client/Player/PlayerPOV.h"
#include "Core/Options/Option.h"
#include "RenderEngine/ChunkRender/Mesh/ChunkMesh.h"
#include "RenderEngine/ChunkRender/TerrainRenderer.h"
#include "Utils/Clock.h"
#include "Utils/Timer/Timer.h"

static thread_local Mesh::ChunkMeshData chunkMesher;

WorldRender::WorldRender(PlayerPOV* player)
    : player_{player}, renderer_{std::make_unique<TerrainRenderer>()} {}

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

  Timer timer;
  chunkMesher.Reset();
  chunkMesher.SetChunk(chunk);
  chunkMesher.GenerateMesh();
  size_t time = static_cast<size_t>(timer.GetTimePassed_μs());

  // Transfer Infomation
  std::unique_ptr<Mesh::ChunkVertexData> data =
      std::make_unique<Mesh::ChunkVertexData>();
  data->solid_vertices_.resize(chunkMesher.solid_face_count_ * 12);
  data->transparent_vertices_.resize(chunkMesher.transparent_face_count_ * 12);
  data->position_ = pos;

  build_stage_0_ += chunkMesher.greedy_time_;
  build_stage_1_ += chunkMesher.cache_time_;
  build_time_ += time;

  ++amount_of_mesh_generated_;

  memcpy(data->solid_vertices_.data(), chunkMesher.vertices_buffer_.data(),
         chunkMesher.solid_face_count_ * 12 * sizeof(uint32_t));

  memcpy(data->transparent_vertices_.data(),
         chunkMesher.transparent_vertices_buffer_.data(),
         chunkMesher.transparent_face_count_ * 12 * sizeof(uint32_t));

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
    // profiler_->CombineCache(worker_output_[(uint64_t)workerId].front()->profiler);
    renderer_->AddChunk(std::move(mesh_add_queue_.back()));
    mesh_add_queue_.pop_back();
    renderer_->Defrag(2);
  }

  LoadChunkMultiToRenderer(updatedChunks);

  renderer_->Defrag(1);

  renderer_->Update();
  renderer_->PrepareRenderer();
}

void WorldRender::Stop() { mesh_thread_pool_->Stop(); }

void WorldRender::Start(GLFWwindow* window, ClientCache* cache,
                        PerformanceProfiler* profiler) {
  horizontal_render_distance_ = g_app_options.horizontal_render_distance_;
  vertical_render_distance_ = g_app_options.vertical_render_distance_;

  size_t threadCount = g_app_options.mesh_threads_;

  mesh_thread_pool_ = std::make_unique<
      ThreadPool<ChunkPos, std::unique_ptr<Mesh::ChunkVertexData>>>(
      threadCount, "Mesher", std::bind_front(&WorldRender::Worker, this), 250);

  window_ = window;
  WorldRender::cache_ = cache;

  renderer_->Initialize(window_, player_->GetCamera());
  renderer_->LoadAssets();
  renderer_->SetSettings(horizontal_render_distance_, vertical_render_distance_,
                         90);

  profiler_ = profiler;
}

size_t WorldRender::GetQueuedSize() {
  return mesh_thread_pool_->GetQueueSize();
}

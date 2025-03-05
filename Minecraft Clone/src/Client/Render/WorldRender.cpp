#include "WorldRender.h"
#include "../../Utils/Clock.h"
#include "../../Core/Options/Option.h"

static thread_local Mesh::ChunkMeshData chunkMesher;

void WorldRender::SetRotation(glm::dvec2 rotation) {
    player_.SetRotation(rotation);
}

void WorldRender::SetPosition(glm::dvec3 position) {
    player_.SetPosition(position);
}

void WorldRender::Render() {
    renderer_v2_.RenderSky();
    renderer_v2_.Render();
}

void WorldRender::LoadChunkToRenderer(ChunkPos chunk) {
    if (server_->GetChunk(chunk) != nullptr) {
        mesh_thread_pool_->SubmitTask(chunk);
    }

}

void WorldRender::LoadChunkMultiToRenderer(std::vector<ChunkPos> chunks) {
    mesh_thread_pool_->SubmitTask(chunks);
}


std::unique_ptr<Mesh::ChunkVertexData> WorldRender::Worker(const ChunkPos& pos) {
    Chunk* chunk = WorldRender::server_->GetChunk(pos);

    Timer timer;
    chunkMesher.Reset();
    chunkMesher.SetChunk(chunk);
    chunkMesher.GenerateMesh();
    size_t time = timer.GetTimePassed_μs();

    //Transfer Infomation
    std::unique_ptr<Mesh::ChunkVertexData> data = std::make_unique<Mesh::ChunkVertexData>();
    data->solidVertices.resize(chunkMesher.solid_face_count_ * 12);
    data->transparentVertices.resize(chunkMesher.transparent_face_count_ * 12);
    data->position_ = pos;
    data->time_ = time;

    memcpy(data->solidVertices.data(), chunkMesher.vertices_buffer_.data(), chunkMesher.solid_face_count_ * 12 * sizeof(uint32_t));
    memcpy(data->transparentVertices.data(), chunkMesher.transparent_vertices_buffer_.data(), chunkMesher.transparent_face_count_ * 12 * sizeof(uint32_t));

    return data;
}

void WorldRender::Update() {
    
    const int chunkUpdateLimit = 4000;
    int updateAmount = 0;

    std::vector<std::unique_ptr<Mesh::ChunkVertexData>> output = mesh_thread_pool_->GetOutput();

    mesh_add_queue_.insert(mesh_add_queue_.end(), std::make_move_iterator(output.begin()), std::make_move_iterator(output.end()));

    while (!mesh_add_queue_.empty()) {
        if (chunkUpdateLimit == updateAmount) {
            break;
        }
        //profiler_->CombineCache(worker_output_[(uint64_t)workerId].front()->profiler);
        build_time_ += mesh_add_queue_.back()->time_;
        renderer_v2_.AddChunk(std::move(mesh_add_queue_.back()));
        mesh_add_queue_.pop_back();
    }

    LoadChunkMultiToRenderer(server_->GetUpdatedChunks());

    if (updateAmount < chunkUpdateLimit) {
        renderer_v2_.Defrag(chunkUpdateLimit - updateAmount);
    }

    renderer_v2_.Update();
    renderer_v2_.PrepareRenderer();

}

void WorldRender::Stop() {
    mesh_thread_pool_->Stop();
    renderer_v2_.Cleanup();
}

void WorldRender::Start(GLFWwindow* window, InternalServer* server, PerformanceProfiler* profiler) {
    horizontal_render_distance_ = g_app_options.horizontal_render_distance_;
    vertical_render_distance_ = g_app_options.vertical_render_distance_;

    int threadCount = g_app_options.mesh_threads_;

    mesh_thread_pool_ = std::make_unique<ThreadPool<ChunkPos, 
        std::unique_ptr<Mesh::ChunkVertexData>, 
        WorldRender::Worker>>(threadCount, "Mesher", 250);

    window_ = window;
    WorldRender::server_ = server;

    renderer_v2_.Initialize(window_, player_.GetCamera());
    renderer_v2_.LoadAssets();
    renderer_v2_.setSettings(horizontal_render_distance_, vertical_render_distance_, 90);

    profiler_ = profiler;
}

size_t WorldRender::GetQueuedSize() {
    return mesh_thread_pool_->GetQueueSize();
}
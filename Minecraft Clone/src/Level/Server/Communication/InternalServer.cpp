#include "InternalServer.h"

void InternalServer::startInternalServer(Player* p) {
    server = new Server();
    player = p;

    ServerSettings settings_;
    settings_.gen_thread_count_ = g_app_options.world_gen_threads_;
    settings_.light_engine_thread_count_ = g_app_options.light_engine_threads_;
    settings_.horizontal_ticking_distance_ = g_app_options.horizontal_render_distance_;
    settings_.vertical_ticking_distance_ = g_app_options.vertical_render_distance_;

    server->StartServer(settings_);
    server->Join(*p);
}

void InternalServer::Stop() {
    server->Stop();
}

glm::vec3 InternalServer::GetPlayerCollusionTimes() {
    return server->GetEntityCollusionTime(player->properties_.entity_uuid_);
}

Chunk* InternalServer::GetChunk(const ChunkPos& chunk) {
    return server->GetChunk(chunk);
}

Timer* InternalServer::GetTickClock() {
    return server->GetTimer();
}

double InternalServer::GetMSPT() {
    return server->GetMSPT();
}

void InternalServer::SetPlayer(Player player) {
    server->Join(player);
}

bool InternalServer::CheckPlayerOnGround() {
    return server->CheckEntityOnGround(player->properties_.entity_uuid_);
}

size_t InternalServer::GetChunkCount() {
    return server->GetChunkCount();
}

std::vector<ChunkPos> InternalServer::GetUpdatedChunks() {
    return server->GetUpdatedChunkPos();
}

std::vector<EntityProperty> InternalServer::GetUpdatedEntities() {
    return server->GetUpdatedEntities();
}

std::vector<EntityUUID>  InternalServer::GetRemovedEntities() {
    return server->GetRemovedEntities();
}

bool InternalServer::CheckRayIntersection(Ray& ray) {
    return server->GetRayIntersection(ray);
}

BlockID InternalServer::GetBlock(const BlockPos& pos) {
    return server->GetBlock(pos);
}

void InternalServer::SendEvent(Event::Event eventIn) {
    server->SendEvent(eventIn);
}
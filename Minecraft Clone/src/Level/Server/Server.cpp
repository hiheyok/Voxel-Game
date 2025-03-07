#include "Server.h"

size_t Server::GetChunkCount() {
    return level_.level_loader_.GetChunkCount();
}

double Server::GetMSPT() {
    return mspt_;
}

Timer* Server::GetTimer() {
    return &time_;
}

std::vector<EntityProperty> Server::GetUpdatedEntities() {
    return level_.main_world_->world_interactions_.GetUpdatedEntities();
}

std::vector<EntityUUID> Server::GetRemovedEntities() {
    return level_.main_world_->world_interactions_.GetRemovedEntities();
}

bool Server::CheckEntityOnGround(EntityUUID id) {
    return level_.main_world_->world_interactions_.collusions_.isEntityOnGround(level_.main_world_->world_interactions_.GetEntity(id));
}

void Server::Join(Entity& entity) {
    level_.main_world_->world_interactions_.summonEntity(entity);
}

std::vector<ChunkPos> Server::GetUpdatedChunkPos() {
    return level_.main_world_->world_interactions_.GetUpdatedChunkPos();
}

Chunk* Server::GetChunk(const ChunkPos& pos) {
    return level_.main_world_->world_interactions_.GetChunk(pos);
}

BlockID Server::GetBlock(const BlockPos& pos) { //Include dimension in parameter later
    return level_.main_world_->world_interactions_.GetBlock(pos);
}

bool Server::GetRayIntersection(Ray& ray) { //Include dimension in paramter later
    return level_.main_world_->world_interactions_.collusions_.CheckRayIntersection(ray);
}

glm::vec3 Server::GetEntityCollusionTime(EntityUUID entity) {
    return level_.main_world_->world_interactions_.collusions_.GetTimeTillCollusion(level_.main_world_->world_interactions_.GetEntity(entity));
}

void Server::StartServer(ServerSettings serverSettings) {
    level_.Start(serverSettings.gen_thread_count_, serverSettings.light_engine_thread_count_);
    stop_ = false;
    settings_ = serverSettings;
    main_server_loop_ = std::thread(&Server::Loop, this);
}

void Server::Stop() {
    stop_ = true;
    main_server_loop_.join();
    level_.Stop();
}

void Server::Loop() {
    g_logger.LogDebug("Server", "Started main server loop");
    while (!stop_) {
        time_.Set();

        Tick();

        mspt_ = time_.GetTimePassed_ms();
        double timeLeft = (1000.0 / (double)settings_.tick_rate_) - mspt_;

        if (timeLeft > 0) {
            timerSleepNotPrecise(static_cast<int>(timeLeft));
        }
    }
    g_logger.LogDebug("Server", "Shutting down main server loop");
}

void Server::Tick() {
    level_.main_world_->Tick();
    level_.main_world_->EventTick();
    level_.updateDimensions();
    level_.main_world_->world_interactions_.worldLoader_->Load();
}

void Server::SendEvent(const Event::Event& pEventIn) {
    level_.main_world_->event_manager_.AddEvent(pEventIn);
}
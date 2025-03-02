#include "InternalServer.h"

using namespace std;
using namespace glm;

void InternalServer::startInternalServer(Player* p) {
	server = new Server();
	player = p;

	ServerSettings settings_;
	settings_.gen_thread_count_ = AppOptions.WorldGenThreads;
	settings_.light_engine_thread_count_ = AppOptions.LightEngineThreads;
	settings_.horizontal_ticking_distance_ = AppOptions.horizontal_render_distance_;
	settings_.vertical_ticking_distance_ = AppOptions.vertical_render_distance_;

	server->StartServer(settings_);
	server->join(*p);
	Block::server_ptr_ = server;

}

void InternalServer::Stop() {
	server->Stop();
}

vec3 InternalServer::GetPlayerCollusionTimes() {
	return server->GetEntityCollusionTime(player->properties_.entity_uuid_);
}

Chunk* InternalServer::GetChunk(const ChunkPos& chunk) {
	return server->GetChunk(chunk);
}

Timer* InternalServer::getTickClock() {
	return server->GetTimer();
}

double InternalServer::getMSPT() {
	return server->getMSPT();
}

void InternalServer::SetPlayer(Player player) {
	server->join(player);
}

bool InternalServer::CheckPlayerOnGround() {
	return server->CheckEntityOnGround(player->properties_.entity_uuid_);
}

int InternalServer::getChunkCount() {
	return server->getChunkCount();
}

vector<ChunkPos> InternalServer::GetUpdatedChunks() {
	return server->GetUpdatedChunkPos();
}

vector<EntityProperty> InternalServer::getUpdatedEntities() {
	return server->getUpdatedEntities();
}

vector<EntityUUID>  InternalServer::getRemovedEntities() {
	return server->getRemovedEntities();
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
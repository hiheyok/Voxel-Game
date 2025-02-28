#include "InternalServer.h"

using namespace std;
using namespace glm;

void InternalServer::startInternalServer(Player* p) {
	server = new Server();
	player = p;

	ServerSettings settings;
	settings.genThreadCount = AppOptions.WorldGenThreads;
	settings.lightEngineThreadCount = AppOptions.LightEngineThreads;
	settings.HorizontalTickingDistance = AppOptions.HorizontalRenderDistance;
	settings.VerticalTickingDistance = AppOptions.VerticalRenderDistance;

	server->startServer(settings);
	server->join(*p);
	Block::serverPTR = server;

}

void InternalServer::Stop() {
	server->Stop();
}

vec3 InternalServer::getPlayerCollusionTimes() {
	return server->getEntityCollusionTime(player->Properties.EntityUUID);
}

Chunk* InternalServer::getChunk(const ChunkPos& chunk) {
	return server->getChunk(chunk);
}

Timer* InternalServer::getTickClock() {
	return server->getTimer();
}

double InternalServer::getMSPT() {
	return server->getMSPT();
}

void InternalServer::setPlayer(Player player) {
	server->join(player);
}

bool InternalServer::checkPlayerOnGround() {
	return server->checkEntityOnGround(player->Properties.EntityUUID);
}

int InternalServer::getChunkCount() {
	return server->getChunkCount();
}

vector<ChunkPos> InternalServer::getUpdatedChunks() {
	vector<ChunkPos> updatedChunkPos = server->getUpdatedChunkPos();

	return updatedChunkPos;
}

vector<EntityProperty> InternalServer::getUpdatedEntities() {
	return server->getUpdatedEntities();
}

vector<EntityUUID>  InternalServer::getRemovedEntities() {
	return server->getRemovedEntities();
}

bool InternalServer::checkRayIntersection(Ray& ray) {
	return server->getRayIntersection(ray);
}

BlockID InternalServer::getBlock(const BlockPos& pos) {
	return server->getBlock(pos);
}

void InternalServer::sendEvent(Event::Event pEventIn) {
	server->sendEvent(pEventIn);
}
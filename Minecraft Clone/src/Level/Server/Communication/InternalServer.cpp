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

Chunk* InternalServer::getChunk(ChunkID chunk) {
	return server->getChunk(chunk);
}

Chunk* InternalServer::getChunk(int x, int y, int z) {
	return getChunk(getChunkID(x, y, z));
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

vector<ChunkID> InternalServer::getUpdatedChunks() {
	vector<ChunkID> updatedChunkIDs = server->getUpdatedChunkIDs();

	return updatedChunkIDs;
}

vector<EntityProperty> InternalServer::getUpdatedEntities() {
	return server->getUpdatedEntities();
}

bool InternalServer::checkRayIntersection(Ray& ray) {
	return server->getRayIntersection(ray);
}

BlockID InternalServer::getBlock(int x, int y, int z) {
	return server->getBlock(x, y, z);
}
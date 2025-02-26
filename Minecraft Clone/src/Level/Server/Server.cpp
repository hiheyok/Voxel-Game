#include "Server.h"

using namespace std;
using namespace glm;

int Server::getChunkCount() {
	return level.levelLoader.getChunkCount();
}

double Server::getMSPT() {
	return MSPT;
}

Timer* Server::getTimer() {
	return &time;
}

vector<EntityProperty> Server::getUpdatedEntities() {
	return level.mainWorld->worldInteractions.getUpdatedEntities();
}

vector<EntityUUID> Server::getRemovedEntities() {
	return level.mainWorld->worldInteractions.getRemovedEntities();
}

bool Server::checkEntityOnGround(EntityUUID id) {
	return level.mainWorld->worldInteractions.Collusions.isEntityOnGround(level.mainWorld->worldInteractions.getEntity(id));
}

void Server::join(Entity& entity) {
	level.mainWorld->worldInteractions.summonEntity(entity);
}

vector<ChunkPos> Server::getUpdatedChunkPoss() {
	return level.mainWorld->worldInteractions.getUpdatedChunkPoss();
}

Chunk* Server::getChunk(const ChunkPos& pos) {
	return level.mainWorld->worldInteractions.getChunk(pos);
}

BlockID Server::getBlock(const BlockPos& pos) { //Include dimension in parameter later
	return level.mainWorld->worldInteractions.getBlock(pos);
}

bool Server::getRayIntersection(Ray& ray) { //Include dimension in paramter later
	return level.mainWorld->worldInteractions.Collusions.CheckRayIntersection(ray);
}

vec3 Server::getEntityCollusionTime(EntityUUID entity) {
	return level.mainWorld->worldInteractions.Collusions.GetTimeTillCollusion(level.mainWorld->worldInteractions.getEntity(entity));
}

void Server::startServer(ServerSettings serverSettings) {
	level.Start(serverSettings.genThreadCount, serverSettings.lightEngineThreadCount);
	stop = false;
	settings = serverSettings;
	mainServerLoop = thread(&Server::loop, this);
}

void Server::Stop() {
	stop = true;
	mainServerLoop.join();
	level.Stop();
}

void Server::loop() {
	Logger.LogDebug("Server", "Started main server loop");
	while (!stop) {
		time.Set();

		tick();

		MSPT = time.GetTimePassed_ms();
		double timeLeft = (1000.0 / (double)settings.tickRate) - MSPT;

		if (timeLeft > 0) {
			timerSleepNotPrecise(timeLeft);
		}
	}
	Logger.LogDebug("Server", "Shutting down main server loop");
}

void Server::tick() {
	level.mainWorld->Tick();
	level.mainWorld->EventTick();
	level.updateDimensions();
	level.mainWorld->worldInteractions.worldLoader->load();
}

void Server::sendEvent(Event pEventIn) {
	level.mainWorld->EventManager.AddEvent(pEventIn);
}
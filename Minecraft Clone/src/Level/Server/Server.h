#pragma once
#include "../Level.h"
#include "../Timer/Timer.h"
#include "../../Utils/Clock.h"
#include <thread>
#include "../../Core/Options/Option.h"

struct ServerSettings {
	int genThreadCount = 8;
	int tickRate = 20;
	int HorizontalTickingDistance = 10;
	int VerticalTickingDistance = 8;
};

//This manages the input/output system of the world
//This also manages the ticking system
class Server {
private:
	Level level;
	bool stop = true;
	std::thread mainServerLoop;
	Timer time;
	double MSPT = 0.0;
public:
	ServerSettings settings;

	Server() {

	}

	int getChunkCount() {
		return level.levelLoader.getChunkCount();
	}

	double getMSPT() {
		return MSPT;
	}

	Timer* getTimer() {
		return &time;
	}

	std::vector<EntityProperty> getUpdatedEntities() {
		return level.mainWorld.worldInteractions.getUpdatedEntities();
	}

	bool checkEntityOnGround(EntityUUID id) {
		return level.mainWorld.worldInteractions.Collusions.isEntityOnGround(level.mainWorld.worldInteractions.getEntity(id));
	}

	void join(Entity& entity) {
		level.mainWorld.worldInteractions.summonEntity(entity);
	}

	std::vector<ChunkID> getUpdatedChunkIDs() {
		return level.mainWorld.worldInteractions.getUpdatedChunkIDs();
	}

	Chunk* getChunk(ChunkID ID) {
		return level.mainWorld.worldInteractions.getChunk(ID);
	}

	BlockID getBlock(int x, int y, int z) { //Include dimension in parameter later
		return level.mainWorld.worldInteractions.getBlock(x, y, z);
	}

	bool getRayIntersection(Ray& ray) { //Include dimension in paramter later
		return level.mainWorld.worldInteractions.Collusions.CheckRayIntersection(ray);
	}

	glm::vec3 getEntityCollusionTime(EntityUUID entity) {
		return level.mainWorld.worldInteractions.Collusions.GetTimeTillCollusion(level.mainWorld.worldInteractions.getEntity(entity));
	}

	void startServer(ServerSettings serverSettings) {
		level.Start(serverSettings.genThreadCount);
		stop = false;
		settings = serverSettings;
		mainServerLoop = std::thread(&Server::loop, this);
	}

	void Stop() {
		stop = true;
		mainServerLoop.join();
		level.Stop();
	}

	void loop() {
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

	void tick() {
		level.mainWorld.Tick();
		level.mainWorld.EventTick();
		level.updateDimensions();
		level.mainWorld.worldInteractions.worldLoader->load();
	}

};
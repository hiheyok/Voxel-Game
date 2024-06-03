#pragma once
#include "../Level.h"
#include "../../World/Server/Time/Timer.h"
#include "../../Utils/Clock.h"
#include <thread>

struct ServerSettings {
	int genThreadCount = 8;
	int tickRate = 20;
};

//This manages the input/output system of the world
//This also manages the ticking system
class Server {
private:
	Level level;
	bool stop = true;
	std::thread mainServerLoop;
	ServerSettings settings;
public:
	Server() {

	}

	void join(Entity& entity) {
		level.mainWorld.worldInteractions.summonEntity(entity);
	}

	std::vector<ChunkID> getUpdatedChunkIDs() {
		level.mainWorld.worldInteractions.getUpdatedChunkIDs();
	}

	Chunk* getChunk(ChunkID ID) {
		return level.mainWorld.worldInteractions.getChunk(ID);
	}

	void startServer(ServerSettings serverSettings) {
		level.Start(serverSettings.genThreadCount);
		stop = false;
		settings = serverSettings;
		mainServerLoop = std::thread(&Server::loop, this);
	}

	void stop() {
		stop = true;
		mainServerLoop.join();
	}

	void loop() {
		while (!stop) {
			Timer time;


			tick();

			double timeLeft = time.GetTimePassed_ms() - (1000.0 / (double)settings.tickRate);

			if (timeLeft > 0) {
				timerSleepNotPrecise(timeLeft);
			}
		}
	}

	void tick() {
		level.mainWorld.Tick();
		level.updateDimensions();
		level.mainWorld.worldInteractions.worldLoader->load();
	}

};
#pragma once
#include "../Level.h"
#include "../Timer/Timer.h"
#include "../../Utils/Clock.h"
#include <thread>
#include "../../Core/Options/Option.h"

struct ServerSettings {
	int genThreadCount = 8;
	int lightEngineThreadCount = 2;
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

	int getChunkCount();

	double getMSPT();

	Timer* getTimer();

	std::vector<EntityProperty> getUpdatedEntities();

	bool checkEntityOnGround(EntityUUID id);

	void join(Entity& entity);

	std::vector<ChunkID> getUpdatedChunkIDs();

	Chunk* getChunk(ChunkID ID);

	BlockID getBlock(int x, int y, int z);

	bool getRayIntersection(Ray& ray);

	glm::vec3 getEntityCollusionTime(EntityUUID entity);

	void startServer(ServerSettings serverSettings);

	void Stop();

	void loop();

	void tick();

};
#pragma once
#include "../Server.h"
#include "../../Entity/Mobs/Player.h"

class InternalServer {
private:
	Server* server;
	Player* player;
public:
	void startInternalServer(Player* p) {
		server = new Server();
		player = p;

		ServerSettings settings;
		settings.genThreadCount = AppOptions.WorldGenThreads;
		settings.HorizontalTickingDistance = AppOptions.HorizontalRenderDistance;
		settings.VerticalTickingDistance = AppOptions.VerticalRenderDistance;

		server->startServer(settings);
		server->join(*p);
		Block::serverPTR = server;

	}

	void Stop() {
		server->Stop();
	}

	glm::vec3 getPlayerCollusionTimes() {
		return server->getEntityCollusionTime(player->Properties.EntityUUID);
	}

	Chunk* getChunk(ChunkID chunk) {
		return server->getChunk(chunk);
	}

	Chunk* getChunk(int x, int y, int z) {
		return getChunk(getChunkID(x, y, z));
	}

	Timer* getTickClock() {
		return server->getTimer();
	}

	double getMSPT() {
		return server->getMSPT();
	}

	void setPlayer(Player player) {
		server->join(player);
	}

	bool checkPlayerOnGround() {
		return server->checkEntityOnGround(player->Properties.EntityUUID);
	}

	int getChunkCount() {
		return server->getChunkCount();
	}

	std::vector<ChunkID> getUpdatedChunks() {
		std::vector<ChunkID> updatedChunkIDs = server->getUpdatedChunkIDs();

		return updatedChunkIDs;
	}

	std::vector<EntityProperty> getUpdatedEntities() {
		return server->getUpdatedEntities();
	}

	bool checkRayIntersection(Ray& ray) {
		return server->getRayIntersection(ray);
	}

	BlockID getBlock(int x, int y, int z) {
		return server->getBlock(x, y, z);
	}

};
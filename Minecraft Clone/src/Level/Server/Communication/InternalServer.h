#pragma once
#include "../Server.h"
#include "../../Entity/Mobs/Player.h"

class InternalServer {
private:
	Server* server;
	Player* player;
public:
	void startInternalServer(Player* p);

	void Stop();

	glm::vec3 getPlayerCollusionTimes();

	Chunk* getChunk(ChunkID chunk);

	Chunk* getChunk(int x, int y, int z);

	Timer* getTickClock();

	double getMSPT();

	void setPlayer(Player player);

	bool checkPlayerOnGround();

	int getChunkCount();

	std::vector<ChunkID> getUpdatedChunks();

	std::vector<EntityProperty> getUpdatedEntities();

	bool checkRayIntersection(Ray& ray);

	BlockID getBlock(int x, int y, int z);

};
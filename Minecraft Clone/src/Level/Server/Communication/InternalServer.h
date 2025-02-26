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

	Chunk* getChunk(const ChunkPos& chunk);

	Timer* getTickClock();

	double getMSPT();

	void setPlayer(Player player);

	bool checkPlayerOnGround();

	int getChunkCount();

	std::vector<ChunkPos> getUpdatedChunks();

	std::vector<EntityProperty> getUpdatedEntities();

	std::vector<EntityUUID> getRemovedEntities();

	bool checkRayIntersection(Ray& ray);

	BlockID getBlock(const BlockPos& pos);

	void sendEvent(Event pEventIn);

};
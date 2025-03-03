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

	glm::vec3 GetPlayerCollusionTimes();

	Chunk* GetChunk(const ChunkPos& chunk);

	Timer* GetTickClock();

	double GetMSPT();

	void SetPlayer(Player player);

	bool CheckPlayerOnGround();

	size_t GetChunkCount();

	std::vector<ChunkPos> GetUpdatedChunks();

	std::vector<EntityProperty> GetUpdatedEntities();

	std::vector<EntityUUID> GetRemovedEntities();

	bool CheckRayIntersection(Ray& ray);

	BlockID GetBlock(const BlockPos& pos);

	void SendEvent(Event::Event pEventIn);

};
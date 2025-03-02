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

	Timer* getTickClock(); // TODO: Rename

	double getMSPT();

	void SetPlayer(Player player);

	bool CheckPlayerOnGround();

	int getChunkCount(); // TODO: Rename

	std::vector<ChunkPos> GetUpdatedChunks();

	std::vector<EntityProperty> getUpdatedEntities(); // TODO: Rename

	std::vector<EntityUUID> getRemovedEntities(); // TODO: Rename

	bool CheckRayIntersection(Ray& ray);

	BlockID GetBlock(const BlockPos& pos);

	void SendEvent(Event::Event pEventIn);

};
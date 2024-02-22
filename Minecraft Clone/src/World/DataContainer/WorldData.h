#pragma once

#include "ChunkMapData.h"
#include "../Entity/Entity.h"
#include "../../Utils/Math/Ray/Ray.h"

/*
Contains all of the world data and basic functions to manage it
	- Chunks
	- Entities
*/

class WorldData {
public:

	BlockID GetBlock(int x, int y, int z);
	bool SetBlock(BlockID block, int x, int y, int z);

	void SetChunk(Chunk* chunk);

	bool CheckChunk(int x, int y, int z);
	bool CheckChunk(ChunkID id);

	Chunk* GetChunk(int x, int y, int z);
	Chunk* GetChunk(ChunkID id);

	bool EraseChunk(int x, int y, int z);

	void ChunkUpdate(int x, int y, int z);

	bool RayIntersection(Ray& ray);

	glm::dvec3 GetTimeTillCollusion(Entity entity);

	bool IsEntityOnGround(Entity entity);

	std::unordered_set<ChunkID> ChunksUpdated;
	std::mutex ChunkUpdateLock;

protected:
	float GetDistanceUntilCollusionSingleDirection(glm::vec3 Origin, int direction, int distancetest);
private:
	size_t tickCount = 0;
	
	std::unordered_map<EntityUUID, Entity> Entities;
	ChunkMap WorldChunkData;
};
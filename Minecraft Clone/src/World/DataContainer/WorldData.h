#pragma once

#include "ChunkMapData.h"
#include "../Entity/Entity.h"
#include "../../Utils/Math/Ray/Ray.h"
#include "EntityContainer.h"

/*
Contains all of the world data and basic functions to manage it
	- Chunks
	- Entities
*/

class WorldData {
public:

	BlockID GetBlock(int x, int y, int z);
	bool SetBlock(BlockID block, int x, int y, int z);

	void AddEntity(Entity& entity);
	void RemoveEntity(EntityUUID entityID);
	Entity* GetEntity(EntityUUID entityID);

	void SetChunk(Chunk* chunk);

	bool CheckChunk(int x, int y, int z);
	bool CheckChunk(ChunkID id);

	Chunk* GetChunk(int x, int y, int z);
	Chunk* GetChunk(ChunkID id);

	bool EraseChunk(int x, int y, int z);

	void ChunkUpdate(int x, int y, int z);

	bool RayIntersection(Ray& ray);

	glm::dvec3 GetTimeTillCollusion(Entity* entity);

	bool IsEntityOnGround(Entity* entity);

	std::unordered_set<ChunkID> ChunksUpdated;
	std::unordered_map<EntityUUID, EntityProperty> EntityUpdated;
	std::unordered_set<EntityUUID> RemovedEntity;
	std::mutex ChunkUpdateLock;
	std::mutex EntityUpdateLock;
	EntityContainer	EntityData;

	float GetDistanceUntilCollusionSingleDirection(glm::vec3 Origin, int direction, int distancetest);
private:
	ChunkMap WorldChunkData;
	
};
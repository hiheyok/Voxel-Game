#pragma once
#include "../World.h"
#include "../WorldParameters.h"
#include "WorldLoader.h"
#include <concurrent_unordered_set.h>
#include <exception>
#include <mutex>
#include <stack>
//Provides a way for the world to be interacted like summoning entities and what chunks are updated and what chunks needs updates
// TODO: IMPORTANT FIX LIGHT UPDATE 

class WorldInteractions {
private:
	World* world = nullptr;
	std::mutex lock;
	std::mutex UpdatedChunkLock;
	FastHashSet<ChunkPos> UpdatedChunk;
	FastHashSet<ChunkPos> RequestedLightUpdate;
public:
	std::vector<ChunkPos> LightUpdateRequest;
	WorldLoader* worldLoader = nullptr;
	WorldParameters settings;
	WorldCollusionDetector Collusions;

	WorldInteractions() {

	}

	WorldInteractions(World* w, WorldParameters parameters) {
		init(w, parameters);
	}

	void UseTallGeneration();

	void init(World* w, WorldParameters parameters);

	void summonEntity(Entity& entity);

	std::vector<ChunkPos> getUpdatedChunkPoss();

	std::vector<ChunkPos> getRequestedLightUpdates();

	std::vector<EntityProperty> getUpdatedEntities();

	std::vector<EntityUUID> getRemovedEntities();

	void requestLightUpdate(const ChunkPos& pos);

	void killEntity(EntityUUID id);

	void update();

	void updateLighting(ChunkLightingContainer* ChunkLighting);

	void updateLighting(std::vector<ChunkLightingContainer*> ChunkLighting);

	void addChunk(Chunk* chunk);

	void addChunks(std::vector<Chunk*> chunks);

	Chunk* getChunk(const ChunkPos& pos) const;

	void setBlock(BlockID b, const BlockPos& pos);

	BlockID getBlock(const BlockPos& pos);

	Entity* getEntity(EntityUUID id);

	void addEntity(Entity& entity);

	void addEntity(Entity* entity);
};

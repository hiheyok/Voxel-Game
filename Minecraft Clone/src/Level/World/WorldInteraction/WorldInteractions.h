#pragma once
#include "../World.h"
#include "../WorldParameters.h"
#include "WorldLoader.h"
#include <concurrent_unordered_set.h>
#include <exception>
#include <mutex>
#include <stack>
//Provides a way for the world to be interacted like summoning entities and what chunks are updated and what chunks needs updates
class WorldInteractions {
private:
	World* world = nullptr;
	std::mutex lock;
	std::mutex UpdatedChunkLock;
	FastHashSet<ChunkID> UpdatedChunk;
	FastHashSet<ChunkColumnID> RequestedLightUpdate;
public:
	std::vector<ChunkColumnID> LightUpdateRequest;
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

	std::vector<ChunkID> getUpdatedChunkIDs();

	std::vector<ChunkID> getRequestedLightUpdates();

	std::vector<EntityProperty> getUpdatedEntities();

	std::vector<EntityUUID> getRemovedEntities();

	void requestLightUpdate(int x, int y, int z);

	void killEntity(EntityUUID id);

	void update();

	void updateLighting(ChunkLightingContainer* ChunkLighting);

	void updateLighting(std::vector<ChunkLightingContainer*> ChunkLighting);

	void addChunk(Chunk* chunk);

	void addChunks(std::vector<Chunk*> chunks);

	Chunk* getChunk(int x, int y, int z);

	Chunk* getChunk(ChunkID ID);

	void setBlock(BlockID b, int x, int y, int z);

	BlockID getBlock(int x, int y, int z);

	Entity* getEntity(EntityUUID id);

	void addEntity(Entity& entity);

	void addEntity(Entity* entity);
};

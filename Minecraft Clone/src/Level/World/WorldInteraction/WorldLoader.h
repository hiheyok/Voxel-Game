#pragma once
#include "../World.h"
#include "../WorldParameters.h"
#include <mutex>

class WorldLoader {
private:
	WorldParameters settings;
	World* world = nullptr;
	std::mutex lock;
	bool isSpawnChunksLoaded = false;

	FastHashSet<EntityUUID> EntityChunkLoaders; //List of entities that force loads chunks 
	FastHashSet<ChunkPos> GeneratingChunk;

	void loadSpawnChunks();

	void loadSurroundedMovedEntityChunk();

	void unloadSurroundedMovedEntityChunk() {

	}

	bool RequestLoad(const ChunkPos& pos);

	void loadSummonEntitySurrounding(EntityUUID uuid);

public:
	bool tallGeneration = false;

	std::vector<ChunkPos> ChunkRequest;

	WorldLoader(World* w, WorldParameters p) : settings(p), world(w) {
	}

	WorldAccess* getWorld();

	void replaceLightInfomation(ChunkLightingContainer* lighting);

	std::vector<ChunkPos> getRequestedChunks();

	void addEntityChunkLoader(EntityUUID uuid);

	void deleteEntityChunkLoader(EntityUUID uuid);

	bool checkEntityExistChunkLoader(EntityUUID uuid);

	void load();

	void addChunk(Chunk* chunk);

};
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
	FastHashSet<ChunkID> GeneratingChunk;

	void loadSpawnChunks();

	void loadSurroundedMovedEntityChunk();

	void unloadSurroundedMovedEntityChunk() {

	}

	bool RequestLoad(int x, int y, int z);

	void loadSummonEntitySurrounding(EntityUUID uuid);

public:
	bool tallGeneration = false;

	std::vector<ChunkID> ChunkRequest;

	WorldLoader(World* w, WorldParameters p) : world(w), settings(p) {
	}

	WorldAccess* getWorld();

	void replaceLightInfomation(ChunkLightingContainer* lighting);

	std::vector<ChunkID> getRequestedChunks();

	void addEntityChunkLoader(EntityUUID uuid);

	void deleteEntityChunkLoader(EntityUUID uuid);

	bool checkEntityExistChunkLoader(EntityUUID uuid);

	void load();

	void addChunk(Chunk* chunk);

};
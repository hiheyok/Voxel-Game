#pragma once
#include "../World.h"
#include "../WorldParameters.h"

class WorldLoader {
private:
	WorldParameters settings;
	World* world = nullptr;

	bool isSpawnChunksLoaded = false;

	std::unordered_set<EntityUUID> EntityChunkLoaders; //List of entities that force loads chunks 

	std::unordered_set<ChunkID> GeneratingChunk;

	void loadSpawnChunks();

	void loadSurroundedMovedEntityChunk();

	void unloadSurroundedMovedEntityChunk() {

	}

	void loadSummonEntitySurrounding(EntityUUID uuid);

public:
	std::vector<ChunkID> ChunkRequest;

	std::vector<ChunkID> getRequestedChunks() {
		std::vector<ChunkID> tmp = ChunkRequest;
		ChunkRequest.clear();
		return tmp;
	}

	WorldLoader(World* w, WorldParameters p) : world(w), settings(p) {

	}

	void addEntityChunkLoader(EntityUUID uuid) {
		EntityChunkLoaders.insert(uuid);
		loadSummonEntitySurrounding(uuid);
	}

	void deleteEntityChunkLoader(EntityUUID uuid) {
		if (!EntityChunkLoaders.count(uuid)) 
			throw std::exception(std::string("Could not find entity with UUID " + std::to_string(uuid)).c_str());

		EntityChunkLoaders.erase(uuid);
	}

	void load() {
		if (!isSpawnChunksLoaded) loadSpawnChunks();

		loadSurroundedMovedEntityChunk();
		unloadSurroundedMovedEntityChunk();
	}

	void addChunk(Chunk* chunk) {
		ChunkID ID = chunk->chunkID;

		if (GeneratingChunk.count(ID)) GeneratingChunk.erase(ID);
		
		world->setChunk(chunk);
	}

};
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

	std::unordered_set<EntityUUID> EntityChunkLoaders; //List of entities that force loads chunks 
	std::unordered_set<ChunkID> GeneratingChunk;

	void loadSpawnChunks();

	void loadSurroundedMovedEntityChunk();

	void unloadSurroundedMovedEntityChunk() {

	}

	void loadSummonEntitySurrounding(EntityUUID uuid);

public:
	std::vector<ChunkID> ChunkRequest;
	
	WorldAccess* getWorld() {
		return static_cast<WorldAccess*>(world);
	}

	void replaceLightInfomation(ChunkLightingContainer* lighting) {
		ChunkColumn* col = world->getColumn(lighting->Position);
		int y = lighting->Position.y & 0b11111;

		col->replaceLightContainer(y, lighting);
	}

	std::vector<ChunkID> getRequestedChunks() {
		std::vector<ChunkID> tmp;
		lock.lock();
		tmp = ChunkRequest; 
		ChunkRequest.clear();
		lock.unlock();
		
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

	bool checkEntityExistChunkLoader(EntityUUID uuid) {
		return EntityChunkLoaders.count(uuid);
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
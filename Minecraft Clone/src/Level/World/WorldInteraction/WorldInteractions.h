#pragma once
#include "../World.h"
#include "../WorldParameters.h"
#include "WorldLoader.h"
#include <unordered_map>
#include <unordered_set>
#include <exception>
//Provides a way for the world to be interacted like summoning entities and what chunks are updated and what chunks needs updates
class WorldInteractions {
private:
	World* world = nullptr;
	WorldParameters settings;

	std::unordered_set<ChunkID> UpdatedChunk; 
public:
	WorldLoader* worldLoader = nullptr;

	WorldInteractions() {

	}

	WorldInteractions(World* w, WorldParameters parameters) {
		world = w;
		settings = parameters;
		worldLoader = new WorldLoader(w, parameters);
	}

	void summonEntity(Entity& entity) {
		if (world == nullptr) throw std::exception("Cannot summon entity. World is null");
		world->Entities.AddEntities(entity);

		if (entity.Properties.isChunkLoader)
			worldLoader->addEntityChunkLoader(entity.Properties.EntityUUID);
	}

	std::vector<ChunkID> getUpdatedChunkIDs() {
		std::vector<ChunkID> chunkIDs = {};

		for (const auto& c : UpdatedChunk) {
			chunkIDs.push_back(c);
		}

		return chunkIDs;
	}

	void killEntity() {

	}

	void update() {
		worldLoader->load();
	}

	void updateLighting(ChunkLightingContainer* ChunkLighting) { //work on this later

	}

	void addChunk(Chunk* chunk) {
		world->setChunk(chunk);
		
		int x = chunk->Position.x;
		int y = chunk->Position.y;
		int z = chunk->Position.z;

		if (!UpdatedChunk.count(getChunkID(x, y, z)))
			UpdatedChunk.insert(getChunkID(x, y, z));
	}

	void addChunks(std::vector<Chunk*> chunks) {
		for (const auto& chunk : chunks) {
			addChunk(chunk);
		}
	}

	Chunk* getChunk(int x, int y, int z) {
		return getChunk(getChunkID(x, y, z));
	}

	Chunk* getChunk(ChunkID ID) {
		if (world->getChunk(ID) == nullptr) throw std::exception(std::string("Chunk with ID " + std::to_string(ID) + "doesn't exist").c_str());

		return world->getChunk(ID);
	}

	void setBlock(BlockID b, int x, int y, int z) {
		try {
			world->setBlock(b, x, y, z);

			if (!UpdatedChunk.count(getChunkID(x >> 4, y >> 4, z >> 4)))
				UpdatedChunk.insert(getChunkID(x >> 4, y >> 4, z >> 4));
			
		}
		catch (std::exception& e) {
			Logger.LogError("World", e.what());
		}
		
	}

	
};

#pragma once
#include "../World.h"
#include "../WorldParameters.h"
#include "WorldLoader.h"
#include <unordered_map>
#include <unordered_set>
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
	//std::unordered_set<ChunkID> UpdatedChunk; 
	std::unordered_set<ChunkID> UpdatedChunk;
public:
	WorldLoader* worldLoader = nullptr;
	WorldParameters settings;
	WorldCollusionDetector Collusions;

	WorldInteractions() {

	}

	WorldInteractions(World* w, WorldParameters parameters) {
		init(w, parameters);
	}

	void init(World* w, WorldParameters parameters) {
		world = w;
		settings = parameters;
		worldLoader = new WorldLoader(w, parameters);
		Collusions.Initialize(static_cast<WorldAccess*>(w));
	}

	void summonEntity(Entity& entity) {
		if (world == nullptr) throw std::exception("Cannot summon entity. World is null");
		world->Entities.AddEntities(entity);

		if (entity.Properties.isChunkLoader)
			worldLoader->addEntityChunkLoader(entity.Properties.EntityUUID);
	}

	std::vector<ChunkID> getUpdatedChunkIDs() {
		std::vector<ChunkID> chunkIDs = {};
		UpdatedChunkLock.lock();
		chunkIDs.insert(chunkIDs.end(), UpdatedChunk.begin(), UpdatedChunk.end());
		UpdatedChunk.clear();
		UpdatedChunkLock.unlock();
		return chunkIDs;
	}

	std::vector<EntityProperty> getUpdatedEntities() {
		std::unordered_map<EntityUUID, EntityProperty> m = world->Entities.ClientGetEntityUpdate();
		std::vector<EntityProperty> properties = {};

		for (const auto& e : m) {
			properties.push_back(e.second);
		}

		return properties;
	}

	std::vector<EntityUUID> getRemovedEntities() {
		std::unordered_set<EntityUUID> m = world->Entities.getRemovedEntities();
		std::vector<EntityUUID> IDs = {};

		IDs.insert(IDs.end(), m.begin(), m.end());

		return IDs;
	}

	void killEntity(EntityUUID id) {
		world->Entities.RemoveEntity(id);
		if (worldLoader->checkEntityExistChunkLoader(id)) {
			worldLoader->deleteEntityChunkLoader(id);
		}
	}

	void update() {
		worldLoader->load();
	}

	void updateLighting(ChunkLightingContainer* ChunkLighting) { //work on this later

	}

	void addChunk(Chunk* chunk) {
		world->setChunk(chunk);
		std::stack<ChunkID> chunkToUpdate;

		int x = chunk->Position.x;
		int y = chunk->Position.y;
		int z = chunk->Position.z;

		chunkToUpdate.push(getChunkID(x, y, z));

		//Update neighbor chunks

		for (int side = 0; side < 6; side ++) {
			int pos[3]{x, y, z};
			pos[side >> 1] += (side & 0b1) * 2 - 1;

			if (world->checkChunk(pos[0], pos[1], pos[2])) {
				chunkToUpdate.push(getChunkID(pos[0], pos[1], pos[2]));
			}
		}

		UpdatedChunkLock.lock();
		while (!chunkToUpdate.empty()) {
			ChunkID id = chunkToUpdate.top();
			chunkToUpdate.pop();
			if (!UpdatedChunk.count(id)) {
				UpdatedChunk.insert(id);
			}

		}
		UpdatedChunkLock.unlock();
	}

	void addChunks(std::vector<Chunk*> chunks) {
		std::stack<ChunkID> chunkToUpdate;

		for (const auto& chunk : chunks) {
			world->setChunk(chunk);

			int x = chunk->Position.x;
			int y = chunk->Position.y;
			int z = chunk->Position.z;

			chunkToUpdate.push(getChunkID(x, y, z));

			//Update neighbor chunks

			for (int side = 0; side < 6; side++) {
				int pos[3]{ x, y, z };
				pos[side >> 1] += (side & 0b1) * 2 - 1;

				if (world->checkChunk(pos[0], pos[1], pos[2])) {
					chunkToUpdate.push(getChunkID(pos[0], pos[1], pos[2]));
				}
			}
		}

		UpdatedChunkLock.lock();
		while (!chunkToUpdate.empty()) {
			ChunkID id = chunkToUpdate.top();
			chunkToUpdate.pop();
			if (!UpdatedChunk.count(id)) {
				UpdatedChunk.insert(id);
			}

		}
		UpdatedChunkLock.unlock();
	}

	Chunk* getChunk(int x, int y, int z) {
		return getChunk(getChunkID(x, y, z));
	}

	Chunk* getChunk(ChunkID ID) {
		if (!world->checkChunk(ID)) {
			return nullptr;
		}

		return world->getChunk(ID);
	}

	void setBlock(BlockID b, int x, int y, int z) {
		try {
			world->setBlock(b, x, y, z);
			UpdatedChunkLock.lock();
			if (!UpdatedChunk.count(getChunkID(x >> 4, y >> 4, z >> 4)))
				UpdatedChunk.insert(getChunkID(x >> 4, y >> 4, z >> 4));
			UpdatedChunkLock.unlock();
		}
		catch (std::exception& e) {
			Logger.LogError("World", e.what());
		}
	}

	BlockID getBlock(int x, int y, int z) {
		try {
			return world->getBlock(x, y, z);
		}
		catch (std::exception& e) {
			Logger.LogError("World", e.what());
			return Blocks.AIR;
		}
	}

	Entity* getEntity(EntityUUID id) {
		return world->Entities.getEntity(id);
	}

	void addEntity(Entity& entity) {
		world->Entities.AddEntities(entity);
	}

	void addEntity(Entity* entity) {
		world->Entities.AddEntities(entity);
	}
};

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
	std::unordered_set<ChunkID> UpdatedChunk;
	std::unordered_set<ChunkColumnID> RequestedLightUpdate;
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

	std::vector<ChunkID> getRequestedLightUpdates() {
		std::vector<ChunkID> columnIDs = {};
		UpdatedChunkLock.lock();
		columnIDs.insert(columnIDs.end(), LightUpdateRequest.begin(), LightUpdateRequest.end());
		LightUpdateRequest.clear();
		UpdatedChunkLock.unlock();
		return columnIDs;
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

	void requestLightUpdate(int x, int y, int z) {
		y = y / 32;

		ChunkColumnID id = getChunkID(x, y, z);
		if (RequestedLightUpdate.count(id)) {
			return;
		}

		UpdatedChunkLock.lock();
		RequestedLightUpdate.emplace(id);
		LightUpdateRequest.push_back(id);
		UpdatedChunkLock.unlock();

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

	void updateLighting(ChunkLightingContainer* ChunkLighting) {
		glm::ivec3 pos = ChunkLighting->Position;

		world->getColumn(pos);
		worldLoader->replaceLightInfomation(ChunkLighting);
		ChunkID id = getChunkID(pos);

		UpdatedChunkLock.lock();
		if (!UpdatedChunk.count(id)) {
			UpdatedChunk.insert(id);
		}
		RequestedLightUpdate.erase(id);
		UpdatedChunkLock.unlock();
	}

	void updateLighting(std::vector<ChunkLightingContainer*> ChunkLighting) {
		std::stack<ChunkID> chunkToUpdate;

		for (const auto& chunk : ChunkLighting) {
			int x = chunk->Position.x;
			int y = chunk->Position.y;
			int z = chunk->Position.z;

			world->getColumn(glm::ivec3(x, y, z));
			worldLoader->replaceLightInfomation(chunk);
			chunkToUpdate.push(getChunkID(x, y, z));
			
		}

		UpdatedChunkLock.lock();
		while (!chunkToUpdate.empty()) {
			ChunkID id = chunkToUpdate.top();
			chunkToUpdate.pop();
			if (!UpdatedChunk.count(id)) {
				UpdatedChunk.insert(id);
			}
			RequestedLightUpdate.erase(id);

		}
		UpdatedChunkLock.unlock();
	}

	void addChunk(Chunk* chunk) {
		world->setChunk(chunk);
		std::stack<ChunkID> chunkToUpdate;

		int x = chunk->Position.x;
		int y = chunk->Position.y;
		int z = chunk->Position.z;

		requestLightUpdate(x, y, z);
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
			requestLightUpdate(x, y, z);
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
			requestLightUpdate(x >> 4, y >> 4, z >> 4);
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

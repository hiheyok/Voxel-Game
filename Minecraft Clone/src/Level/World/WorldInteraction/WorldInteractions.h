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
	std::mutex updated_chunk_lock_;
	FastHashSet<ChunkPos> updated_chunk_;
	FastHashMap<ChunkPos, std::pair<int, size_t>> requested_light_update_; // pair<y height, index in update vector>
	std::vector<ChunkPos> light_updates_;
public:
	WorldLoader* worldLoader_ = nullptr;
	WorldParameters settings_;
	WorldCollusionDetector collusions_;

	WorldInteractions() {

	}

	WorldInteractions(World* w, WorldParameters parameters) {
		init(w, parameters);
	}

	void UseTallGeneration();

	void init(World* w, WorldParameters parameters);

	void summonEntity(Entity& entity);

	std::vector<ChunkPos> GetUpdatedChunkPos();

	std::vector<ChunkPos> GetRequestedLightUpdates();

	std::vector<EntityProperty> GetUpdatedEntities(); // TODO: Rename

	std::vector<EntityUUID> GetRemovedEntities(); // TODO: Rename

	void RequestLightUpdate(const ChunkPos& pos);

	void KillEntity(EntityUUID id);

	void Update();

	void UpdateLighting(std::shared_ptr<ChunkLightingContainer> chunkLighting);

	void UpdateLighting(std::vector<std::shared_ptr<ChunkLightingContainer>> chunkLighting);

	void AddChunk(Chunk* chunk);

	void AddChunks(std::vector<Chunk*> chunks);

	Chunk* GetChunk(const ChunkPos& pos) const;

	void SetBlock(BlockID b, const BlockPos& pos);

	BlockID GetBlock(const BlockPos& pos);

	Entity* GetEntity(EntityUUID id);

	void AddEntity(Entity& entity);

	void AddEntity(Entity* entity);
};

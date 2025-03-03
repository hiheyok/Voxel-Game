#pragma once
#include "../World.h"
#include "../WorldParameters.h"
#include <mutex>

class WorldLoader {
private:
	WorldParameters settings_;
	World* world = nullptr;
	std::mutex lock_;
	bool is_spawn_chunks_loaded_ = false;

	FastHashSet<EntityUUID> entity_chunk_loaders_; //List of entities that force loads chunks 
	FastHashSet<ChunkPos> generating_chunk_;

	void loadSpawnChunks();

	void loadSurroundedMovedEntityChunk();

	void unloadSurroundedMovedEntityChunk() {

	}

	bool RequestLoad(const ChunkPos& pos);

	void loadSummonEntitySurrounding(EntityUUID uuid);

public:
	bool tall_generation_ = false;

	std::vector<ChunkPos> chunk_request_;

	WorldLoader(World* w, WorldParameters p) : settings_(p), world(w) {}

	WorldAccess* GetWorld() const;

	void ReplaceLightInfomation(std::shared_ptr<ChunkLightingContainer> lighting);

	std::vector<ChunkPos> GetRequestedChunks();

	void AddEntityChunkLoader(EntityUUID uuid);

	void DeleteEntityChunkLoader(EntityUUID uuid);

	bool CheckEntityExistChunkLoader(EntityUUID uuid) const;

	void Load();

	void AddChunk(Chunk* chunk);

};
#pragma once
#include <memory>
#include <mutex>

#include "Core/Typenames.h"

class World;
class Chunk;
class LightStorage;

struct Entity;
struct EntityProperty;
struct WorldParameters;

// Add chunk unloading later when entity goes out of range
class WorldUpdater {
   public:
    bool tall_generation_ = false;

    WorldUpdater(World* w, WorldParameters p);

    void DeleteEntityChunkLoader(EntityUUID uuid);
    bool CheckEntityExistChunkLoader(EntityUUID uuid) const;
    void Load();

    // Getters
    std::vector<ChunkPos> GetUpdatedChunkPos();
    std::vector<ChunkPos> GetUpdatedLightPos();
    std::vector<ChunkPos> GetRequestedLightUpdates();
    std::vector<ChunkPos> GetRequestedChunks();

    std::vector<EntityProperty> GetSpawnedEntities();
    std::vector<EntityProperty> GetUpdatedEntities();
    std::vector<EntityUUID> GetRemovedEntities();

    // Setters
    void SetLight(std::unique_ptr<LightStorage> light);
    void SetChunk(std::unique_ptr<Chunk> chunk);
    void SetLight(std::vector<std::unique_ptr<LightStorage>> lights);
    void SetChunk(std::vector<std::unique_ptr<Chunk>> chunks);
    EntityUUID SetEntity(std::unique_ptr<Entity> entity);
    void SetEntityChunkLoader(EntityUUID uuid);
    void SetBlock(const BlockID& block, const BlockPos& pos);
    void KillEntity(const EntityUUID& uuid);

   private:
    bool RequestLoad(const ChunkPos& pos);
    void loadSpawnChunks();
    void loadSummonEntitySurrounding(EntityUUID uuid);
    void loadSurroundedMovedEntityChunk();

    std::unique_ptr<WorldParameters> settings_;
    World* world_ = nullptr;
    std::mutex lock_;
    std::mutex other_lock_;  // TODO: tmp fix this later
    bool is_spawn_chunks_loaded_ = false;
    std::vector<ChunkPos> chunk_request_;

    FastHashSet<EntityUUID>
        entity_chunk_loaders_;  // List of entities that force loads chunks
    FastHashSet<ChunkPos> generating_chunk_;

    std::mutex updated_chunk_lock_;
    FastHashSet<ChunkPos> updated_chunk_;
    std::vector<ChunkPos> updated_chunk_arr_;
    std::mutex updated_light_lock_;
    FastHashSet<ChunkPos> updated_light_;
    std::vector<ChunkPos> updated_light_arr_;
};
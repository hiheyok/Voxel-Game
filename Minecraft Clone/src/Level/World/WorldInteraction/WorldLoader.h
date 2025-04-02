#pragma once
#include <mutex>
#include <memory>

#include "Core/Typenames.h"

class World;
class Chunk;
class LightStorage;

struct WorldAccess;
struct WorldParameters;

// Add chunk unloading later when entity goes out of range
class WorldLoader {
private:
    std::unique_ptr<WorldParameters> settings_;
    World* world = nullptr;
    std::mutex lock_;
    std::mutex other_lock_; // TODO: tmp fix this later
    bool is_spawn_chunks_loaded_ = false;
    std::vector<ChunkPos> chunk_request_;

    FastHashSet<EntityUUID> entity_chunk_loaders_; //List of entities that force loads chunks 
    FastHashSet<ChunkPos> generating_chunk_;

    void loadSpawnChunks();

    void loadSurroundedMovedEntityChunk();

    bool RequestLoad(const ChunkPos& pos);

    void loadSummonEntitySurrounding(EntityUUID uuid);

public:
    bool tall_generation_ = false;

    WorldLoader(World* w, WorldParameters p);

    WorldAccess* GetWorld() const;

    void ReplaceLightInfomation(std::unique_ptr<LightStorage> lighting);

    std::vector<ChunkPos> GetRequestedChunks();

    void AddEntityChunkLoader(EntityUUID uuid);

    void DeleteEntityChunkLoader(EntityUUID uuid);

    bool CheckEntityExistChunkLoader(EntityUUID uuid) const;

    void Load();

    void AddChunk(std::unique_ptr<Chunk> chunk);

};
#pragma once
#include <mutex>
#include <memory>
#include <stack>
#include <vector>

#include "Core/Typenames.h"

// Provides a way for the world to be interacted like summoning entities and what chunks are updated and what chunks needs updates
// TODO: IMPORTANT FIX LIGHT UPDATE 

class World;
class WorldLoader;
class CollusionDetector;
class Chunk;
class LightStorage;

struct WorldParameters;
struct Entity;
struct EntityProperty;

class WorldInteractions {
private:
    World* world = nullptr;
    std::mutex updated_chunk_lock_;
    FastHashSet<ChunkPos> updated_chunk_;
    FastHashSet<ChunkPos> updated_lighting_pos_;
    std::vector<std::pair<BlockID, BlockPos>> block_updates_;
public:
    std::unique_ptr<WorldLoader> worldLoader_;
    std::unique_ptr<WorldParameters> settings_;
    std::unique_ptr<CollusionDetector> collusions_;

    WorldInteractions();
    ~WorldInteractions();

    WorldInteractions(World* w, WorldParameters parameters);

    void UseTallGeneration();

    void init(World* w, WorldParameters parameters);

    std::vector<ChunkPos> GetUpdatedChunkPos();
    std::vector<ChunkPos> GetUpdatedLightPos();

    std::vector<ChunkPos> GetRequestedLightUpdates();

    std::vector<EntityProperty> GetSpawnedEntities();
    std::vector<EntityProperty> GetUpdatedEntities();
    std::vector<EntityUUID> GetRemovedEntities();

    void KillEntity(EntityUUID id);

    void Update();

    void UpdateLighting(std::vector<std::unique_ptr<LightStorage>> chunkLighting);

    void AddChunk(std::unique_ptr<Chunk> chunk);

    void AddChunks(std::vector<std::unique_ptr<Chunk>> chunks);

    Chunk* GetChunk(const ChunkPos& pos) const;

    void SetBlock(BlockID b, const BlockPos& pos);

    BlockID GetBlock(const BlockPos& pos);

    Entity* GetEntity(EntityUUID id);

    EntityUUID AddEntity(std::unique_ptr<Entity> entity);
};

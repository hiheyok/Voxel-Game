#pragma once
#include <mutex>
#include <memory>
#include <stack>
#include <vector>

#include "../../Typenames.h"

// Provides a way for the world to be interacted like summoning entities and what chunks are updated and what chunks needs updates
// TODO: IMPORTANT FIX LIGHT UPDATE 

class World;
class WorldLoader;
class WorldCollusionDetector;
class Chunk;
class ChunkLightingContainer;

struct WorldParameters;
struct Entity;
struct EntityProperty;

class WorldInteractions {
private:
    World* world = nullptr;
    std::mutex updated_chunk_lock_;
    FastHashSet<ChunkPos> updated_chunk_;
    FastHashSet<ChunkPos> updated_lighting_pos_;
    FastHashMap<ChunkPos, std::pair<int, size_t>> requested_light_update_; // pair<y height, index in update vector>
    std::vector<ChunkPos> light_updates_;
public:
    WorldLoader* worldLoader_ = nullptr;
    std::unique_ptr<WorldParameters> settings_;
    std::unique_ptr<WorldCollusionDetector> collusions_;

    WorldInteractions();

    WorldInteractions(World* w, WorldParameters parameters);

    void UseTallGeneration();

    void init(World* w, WorldParameters parameters);

    std::vector<ChunkPos> GetUpdatedChunkPos();
    std::vector<ChunkPos> GetUpdatedLightPos();

    std::vector<ChunkPos> GetRequestedLightUpdates();

    std::vector<EntityProperty> GetSpawnedEntities();
    std::vector<EntityProperty> GetUpdatedEntities();
    std::vector<EntityUUID> GetRemovedEntities();

    void RequestLightUpdate(const ChunkPos& pos);

    void KillEntity(EntityUUID id);

    void Update();

    void UpdateLighting(std::unique_ptr<ChunkLightingContainer> chunkLighting);

    void UpdateLighting(std::vector<std::unique_ptr<ChunkLightingContainer>> chunkLighting);

    void AddChunk(std::unique_ptr<Chunk> chunk);

    void AddChunks(std::vector<std::unique_ptr<Chunk>> chunks);

    Chunk* GetChunk(const ChunkPos& pos) const;

    void SetBlock(BlockID b, const BlockPos& pos);

    BlockID GetBlock(const BlockPos& pos);

    Entity* GetEntity(EntityUUID id);

    EntityUUID AddEntity(std::unique_ptr<Entity> entity);
};

#pragma once
#include <memory>
#include <vector>

#include "Core/Typenames.h"
#include "Level/World/WorldParameters.h"

class EntityContainer;
class ChunkMap;
class Chunk;

struct Entity;

// Allows for read only
class WorldInterface {
public:
    WorldInterface();
    virtual ~WorldInterface();

    BlockID GetBlock(const BlockPos& pos) const;
    Entity* GetEntity(EntityUUID) const;
    Chunk* GetChunk(const ChunkPos& ID) const;
    bool CheckChunk(const ChunkPos& ID) const;
    std::vector<Chunk*> GetAllChunks() const;

    ChunkMap* GetChunkMap() const;
    EntityContainer* GetEntityContainer() const;

    WorldParameters parameters;
protected:
    std::unique_ptr<EntityContainer> entities_;
    std::unique_ptr<ChunkMap> chunks_;
};
#pragma once
#include <memory>

#include "Core/Typenames.h"

class ChunkMap;
class EntityContainer;
class Heightmap;
class ChunkColumn;
class Chunk;

struct WorldAccess {
    std::unique_ptr<ChunkMap> chunk_container_;
    std::unique_ptr<EntityContainer> entities_; //Remove tick later, this is going to be specific to dimension stuff

    WorldAccess();
    ~WorldAccess();

    BlockID GetBlock(const BlockPos& pos);
    
    void SetBlock(BlockID block, const BlockPos& pos);

    void SetChunk(std::unique_ptr<Chunk> chunk);

    Chunk* GetChunk(const ChunkPos& ID) const;

    bool CheckChunk(const ChunkPos& ID) const;
    
    Heightmap& GetColumnHeightmap(const ChunkColumnPos& v) const;

    ChunkColumn* GetColumn(const ChunkPos& v) const;
};
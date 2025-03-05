#pragma once
#include "../DataContainer/ChunkMapData.h"
#include "../DataContainer/EntityContainer.h"
#include <exception>
#include <glm/vec3.hpp>

class WorldAccess {
private:
    ChunkMap chunk_container_;
public:
    EntityContainer entities_; //Remove tick later, this is going to be specific to dimension stuff

    BlockID GetBlock(const BlockPos& pos);
    
    void SetBlock(BlockID block, const BlockPos& pos);

    void SetChunk(Chunk* chunk);

    Chunk* GetChunk(const ChunkPos& ID) const;

    bool CheckChunk(const ChunkPos& ID) const;
    
    Heightmap& GetColumnHeightmap(const ChunkColumnPos& v) const;

    ChunkColumn* GetColumn(const ChunkPos& v) const;
};
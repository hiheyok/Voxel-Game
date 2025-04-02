#pragma once
#include <memory>

class Chunk;
class ClientRegion;

#include "Core/Typenames.h"

class ClientChunkCache {
public:
    ClientChunkCache();
    ~ClientChunkCache();

    BlockID GetBlock(const BlockPos& pos) const;
    void SetBlock(BlockID block, const BlockPos& pos);

    bool CheckChunk(const ChunkPos& pos) const;
    void InsertChunk(std::unique_ptr<Chunk> chunk);
    void EraseChunk(const ChunkPos& pos);
    Chunk* GetChunk(const ChunkPos& pos) const;
private:
    ClientRegion* GetRegion(const RegionPos& pos) const;
    bool CheckRegion(const RegionPos& pos) const;
    void CreateRegion(const RegionPos& pos);
    void DeleteRegion(const RegionPos& pos);

    FastHashMap<RegionPos, std::unique_ptr<ClientRegion>> regions_;
};
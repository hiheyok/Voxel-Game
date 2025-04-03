#pragma once
#include <vector>
#include <memory>

#include "Core/Typenames.h"

class Chunk;

struct Region { //32x32x32 Chunk Region
    //ChunkColumn* region_[32 * 32]{ nullptr };
    std::vector<std::unique_ptr<Chunk>> region_;
    Region();
    ~Region();

    void AddChunk(std::unique_ptr<Chunk> chunk);

    void EraseChunk(int x, int y, int z);

    bool CheckChunk(int x, int y, int z) const;

    Chunk* GetChunk(int x, int y, int z) const;

    uint64_t access_count_ = 0;
};

class ChunkMap {
public:
    bool CheckChunk(const ChunkPos& pos) const;

    BlockID GetBlock(const BlockPos& pos) const;

    bool SetBlock(BlockID block, const BlockPos& pos);

    void InsertChunk(std::unique_ptr<Chunk> chunk);

    bool EraseChunk(const ChunkPos& pos);

    Chunk* GetChunk(const ChunkPos& pos) const;
private:
    //Input position is the chunk position
    Region* GetRegion(const ChunkPos& pos) const;
    bool CheckRegion(const ChunkPos& pos) const;

    FastHashMap<RegionPos, std::unique_ptr<Region>> live_region_;
};
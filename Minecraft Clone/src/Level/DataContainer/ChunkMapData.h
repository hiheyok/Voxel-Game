#pragma once
#include <vector>
#include <memory>

#include "../Typenames.h"

class ChunkColumn;
class Chunk;

struct Region { //32x32x32 Chunk Region
    //ChunkColumn* region_[32 * 32]{ nullptr };
    std::vector<std::unique_ptr<ChunkColumn>> region_;
    Region();
    ~Region();

    void AddChunk(std::unique_ptr<Chunk> chunk, int x, int y, int z);
    void AddChunkGlobalPos(std::unique_ptr<Chunk> chunk, int x, int y, int z);

    void EraseChunk(int x, int y, int z);
    void EraseChunkGlobalPos(int x, int y, int z);

    bool CheckChunk(int x, int y, int z) const;
    bool CheckChunkGlobalPos(int x, int y, int z) const;

    Chunk* GetChunk(int x, int y, int z) const;
    Chunk* GetChunkGlobalPos(int x, int y, int z) const;

    ChunkColumn* GetChunkColumn(int x, int z) const;
    ChunkColumn* GetChunkColumnGlobalPos(int x, int z) const;

    bool CheckChunkColumn(int x, int z) const;
    bool CheckChunkColumnGlobalPos(int x, int z) const;

    uint64_t access_count_ = 0;
};

class ChunkMap {
public:

    ChunkColumn* GetColumn(const ChunkPos& pos) const;

    bool CheckChunk(const ChunkPos& pos) const;

    BlockID GetBlockGlobal(const BlockPos& pos) const;

    bool SetBlockGlobal(BlockID block, const BlockPos& pos);

    void InsertChunk(std::unique_ptr<Chunk> chunk);

    bool EraseChunk(const ChunkPos& pos);

    Chunk* GetChunk(const ChunkPos& pos) const;
private:
    //Input position is the chunk position
    const std::unique_ptr<Region>& GetRegion(const ChunkPos& pos) const;
    bool CheckRegion(const ChunkPos& pos) const;

    FastHashMap<RegionPos, std::unique_ptr<Region>> live_region_;
};
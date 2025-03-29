#pragma once
#include <vector>
#include "../Chunk/Chunk.h"
#include "../Chunk/ChunkColumn.h"
#include "../../Utils/Containers/skaHashmap.h"

typedef unsigned long long int RegionID;

struct Region { //32x32x32 Chunk Region
    //ChunkColumn* region_[32 * 32]{ nullptr };
    std::vector<std::unique_ptr<ChunkColumn>> region_;
    Region();
    ~Region();

    void AddChunk(std::unique_ptr<Chunk> chunk, uint16_t x, uint16_t y, uint16_t z);
    void AddChunkGlobalPos(std::unique_ptr<Chunk> chunk, int32_t x, int32_t y, int32_t z);

    void EraseChunk(uint16_t x, uint16_t y, uint16_t z);
    void EraseChunkGlobalPos(int32_t x, int32_t y, int32_t z);

    bool CheckChunk(uint16_t x, uint16_t y, uint16_t z) const;
    bool CheckChunkGlobalPos(int32_t x, int32_t y, int32_t z) const;

    Chunk* GetChunk(uint16_t x, uint16_t y, uint16_t z) const;
    Chunk* GetChunkGlobalPos(int32_t x, int32_t y, int32_t z) const;

    ChunkColumn* GetChunkColumn(uint16_t x, uint16_t z) const;
    ChunkColumn* GetChunkColumnGlobalPos(int32_t x, int32_t z) const;

    bool CheckChunkColumn(uint16_t x, uint16_t z) const;
    bool CheckChunkColumnGlobalPos(int32_t x, int32_t z) const;

    uint64_t access_count_ = 0;
};

class ChunkMap {
public:

    ChunkColumn* GetColumn(const ChunkPos& pos) const;

    bool CheckChunk(const ChunkPos& pos) const;

    BlockID GetBlockGlobal(const BlockPos& pos);

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
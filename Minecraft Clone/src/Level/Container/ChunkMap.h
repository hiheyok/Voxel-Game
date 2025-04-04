#pragma once
#include <memory>
#include <utility>

class Chunk;
class Region;

#include "Core/Typenames.h"

class ChunkMap {
public:
    ChunkMap(bool heightmapUpdate = false);
    ~ChunkMap();

    BlockID GetBlock(const BlockPos& pos) const;
    bool SetBlock(BlockID block, const BlockPos& pos);

    bool CheckChunk(const ChunkPos& pos) const;
    void InsertChunk(std::unique_ptr<Chunk> chunk);
    void EraseChunk(const ChunkPos& pos);
    Chunk* GetChunk(const ChunkPos& pos) const;
    std::vector<Chunk*> GetAllChunks() const;
private:
    Region* GetRegion(const RegionPos& pos) const;
    Region* GetRegionUncheck(const RegionPos& pos) const;
    bool CheckRegion(const RegionPos& pos, bool checkCache = true) const;
    Region* CreateRegion(const RegionPos& pos);
    void DeleteRegion(const RegionPos& pos);

    bool heightmap_update_ = false;

    constexpr const static int kRegionCacheSize = 16;

    std::vector<Chunk*> chunks_;
    FastHashMap<ChunkPos, size_t> chunks_idx_;
    FastHashMap<RegionPos, std::unique_ptr<Region>> regions_;
    mutable std::vector<std::pair<RegionPos, Region*>> region_cache_;
};
#include "Level/Container/ChunkMap.h"
#include "Level/Container/Region.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/ChunkRawData.h"

ChunkMap::ChunkMap(bool heightmapUpdate) : heightmap_update_{ heightmapUpdate } {};
ChunkMap::~ChunkMap() = default;

void ChunkMap::EraseChunk(const ChunkPos& pos) {
    RegionPos regPos = pos / kRegionDim;
    Region* reg = GetRegion(regPos);

    for (int axis_ = 0; axis_ < 3; axis_++) {
        for (int face = 0; face < 2; face++) {
            ChunkPos posNeighbor = pos;
            posNeighbor.incrementSide(axis_ * 2 + face, 1);

            if (CheckChunk(posNeighbor)) {
                GetChunk(posNeighbor)->SetNeighbor(nullptr, axis_ * 2 - face + 1);
            }
        }
    }

    // erase chunk from chunks_
    Chunk* swappedChunk = chunks_.back();
    
    int idx = chunks_idx_[pos];
    chunks_idx_[swappedChunk->position_] = idx;
    chunks_idx_.erase(pos);
    std::swap(chunks_[idx], chunks_.back());
    chunks_.pop_back();

    ChunkContainer* bottomChunk = reg->GetChunk(pos)->GetNeighbor(NY);

    reg->EraseChunk(pos);

    if (heightmap_update_) {
        bottomChunk->UpdateHeightMap();
    }

    // Delete region if it is empty
    if (reg->GetChunkCount() == 0) {
        DeleteRegion(regPos);
    }
}


bool ChunkMap::SetBlock(BlockID block, const BlockPos& pos) {
    ChunkPos chunkPos = pos / kChunkDim;
    BlockPos localPos = pos & (kChunkDim - 1);
    RegionPos regPos = chunkPos / kRegionDim;

    Region* reg = GetRegionUncheck(regPos);
    if (reg == nullptr) return false;

    Chunk* chunk = reg->GetChunk(chunkPos);

    if (chunk != nullptr) {
        chunk->SetBlockUnsafe(block, localPos.x, localPos.y, localPos.z);
        chunk->is_empty_ = false;

        if (heightmap_update_) {
            chunk->UpdateHeightMap(localPos.x, localPos.z);
        }

        return true;
    }

    return false;
}

BlockID ChunkMap::GetBlock(const BlockPos& pos) const {
    ChunkPos chunkPos = pos / kChunkDim;
    BlockPos localBlockPos = pos & (kChunkDim - 1);
    RegionPos regPos = chunkPos / kRegionDim;

    Region* reg = GetRegionUncheck(regPos);
    if (reg == nullptr) return g_blocks.AIR;

    if (!reg->CheckChunk(chunkPos)) {
        return g_blocks.AIR;
    }

    Chunk* chunk = reg->GetChunk(chunkPos);

    return chunk->GetBlock(localBlockPos.x, localBlockPos.y, localBlockPos.z);
}

bool ChunkMap::CheckChunk(const ChunkPos& pos) const {
    RegionPos regPos = pos / kRegionDim;
    Region* reg = GetRegionUncheck(regPos);
    if (reg == nullptr) return false;
    return reg->CheckChunk(pos);
}

Chunk* ChunkMap::GetChunk(const ChunkPos& pos) const {
    RegionPos regPos = pos / kRegionDim;
    Region* reg = GetRegionUncheck(regPos);
    if (reg == nullptr) return nullptr;

    return reg->GetChunk(pos);
}

std::vector<Chunk*> ChunkMap::GetAllChunks() const {
    return chunks_;
}

void ChunkMap::InsertChunk(std::unique_ptr<Chunk> chunk) {
    const ChunkPos pos = chunk->position_;

    RegionPos regPos = pos / kRegionDim;
    Region* reg = GetRegionUncheck(regPos);

    if (reg == nullptr) {
        reg = CreateRegion(regPos);
    }


    for (int axis_ = 0; axis_ < 3; axis_++) {
        for (int face = 0; face < 2; face++) {
            ChunkPos posNeighbor = chunk->position_;
            posNeighbor.incrementSide(axis_ * 2 + face, 1);

            if (CheckChunk(posNeighbor)) {
                chunk->SetNeighbor(static_cast<ChunkContainer*>(GetChunk(posNeighbor)), axis_ * 2 + face);
                GetChunk(posNeighbor)->SetNeighbor(static_cast<ChunkContainer*>(chunk.get()), axis_ * 2 - face + 1);
                GetChunk(posNeighbor)->UpdateGen();
            }
        }
    }

    chunk->UpdateGen();

    // If there exist a chunk then replace its content
    if (reg->CheckChunk(pos)) {
        Chunk* oldChunk = reg->GetChunk(pos);
        oldChunk->SetData(chunk->GetRawData());
    } else {
        chunks_.push_back(chunk.get());
        chunks_idx_.emplace(chunk->position_, chunks_.size() - 1);
        reg->InsertChunk(std::move(chunk));
    }

    Chunk* chunkPtr = reg->GetChunk(pos);

    if (heightmap_update_) {
        chunkPtr->UpdateHeightMap();
    }
}

Region* ChunkMap::GetRegion(const RegionPos& pos) const {
    // First try to find region in the cache 
    size_t minUsageCount = SIZE_MAX;
    int minIdx = -1;

    for (int i = 0; i < region_cache_.size(); ++i) {
        Region* currRegion = region_cache_[i].second;

        size_t currUsage = currRegion->GetUsageCount();
        if (region_cache_[i].first == pos) {
            currRegion->IncrementUsage();
            return currRegion;
        }
        if (minUsageCount > currUsage) {
            minUsageCount = currUsage;
            minIdx = i;
        }
    }
    
    const auto& it = regions_.find(pos);

    if (it == regions_.end()) { // don't the cache because it is not in there
        g_logger.LogError("ChunkMap::GetRegion", "Tried to get region that doesn't exist!");
    }
    Region* region = it->second.get();
    
    if (region_cache_.size() <= kRegionCacheSize) {
        region_cache_.emplace_back(pos, region);
        return region;
    }

    // Check if the region is less than the min
    if (minUsageCount < region->GetUsageCount()) { // replace the min with the more used region
        region_cache_[minIdx] = { pos, region };
    }
    
    return region;
}


Region* ChunkMap::GetRegionUncheck(const RegionPos& pos) const {
    // First try to find region in the cache 
    size_t minUsageCount = SIZE_MAX;
    int minIdx = -1;

    for (int i = 0; i < region_cache_.size(); ++i) {
        Region* currRegion = region_cache_[i].second;

        size_t currUsage = currRegion->GetUsageCount();
        if (region_cache_[i].first == pos) {
            currRegion->IncrementUsage();
            return currRegion;
        }
        if (minUsageCount > currUsage) {
            minUsageCount = currUsage;
            minIdx = i;
        }
    }

    const auto& it = regions_.find(pos);

    if (it == regions_.end()) { 
        return nullptr;
    }
    Region* region = it->second.get();

    if (region_cache_.size() <= kRegionCacheSize) {
        region_cache_.emplace_back(pos, region);
        return region;
    }

    // Check if the region is less than the min
    if (minUsageCount < region->GetUsageCount()) { // replace the min with the more used region
        region_cache_[minIdx] = { pos, region };
    }

    return region;
}

bool ChunkMap::CheckRegion(const RegionPos& pos, bool checkCache) const {
    if (checkCache) {
        // Tries to find it in the cache first
        size_t minUsageCount = SIZE_MAX;
        int minIdx = -1;

        for (int i = 0; i < region_cache_.size(); ++i) {
            Region* currRegion = region_cache_[i].second;

            size_t currUsage = currRegion->GetUsageCount();
            if (region_cache_[i].first == pos) {
                currRegion->IncrementUsage();
                return currRegion;
            }
            if (minUsageCount > currUsage) {
                minUsageCount = currUsage;
                minIdx = i;
            }
        }

        const auto& it = regions_.find(pos);
        if (it == regions_.end()) return false;

        Region* region = it->second.get();
        region->IncrementUsage();

        if (region_cache_.size() <= kRegionCacheSize) {
            region_cache_.emplace_back(pos, region);
            return true;
        }

        if (region->GetUsageCount() > minUsageCount) {
            region_cache_[minIdx] = { pos, region };
        }


        return true;

    } else {
        return regions_.contains(pos);
    }
}

Region* ChunkMap::CreateRegion(const RegionPos& pos) {
    if (CheckRegion(pos)) {
        g_logger.LogError("ChunkMap::CreateRegion", "Tried to create region when one already exist!");
    }
    std::unique_ptr<Region> newRegion = std::make_unique<Region>();
    Region* region = newRegion.get();
    regions_.emplace(pos, std::move(newRegion));
    return region;
}

void ChunkMap::DeleteRegion(const RegionPos& pos) {
    if (!CheckRegion(pos)) {
        g_logger.LogError("ChunkMap::DeleteRegion", "Tried to delete region that is already deleted!");
    }
    // Tries to delete from cache
    for (int i = 0; i < region_cache_.size(); ++i) {
        if (region_cache_[i].first == pos) {
            std::swap(region_cache_[i], region_cache_.back());
            region_cache_.pop_back();
            break;
        }
    }

    regions_.erase(pos);
}


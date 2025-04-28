#include "Level/Container/ChunkMap.h"

#include <limits>

#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/ChunkRawData.h"
#include "Level/Container/Region.h"

ChunkMap::ChunkMap(bool neighborUpdate, bool heightmapUpdate)
    : neighbor_update_{neighborUpdate}, heightmap_update_{heightmapUpdate} {};
ChunkMap::~ChunkMap() = default;

void ChunkMap::EraseChunk(const ChunkPos& pos) {
    RegionPos reg_pos = pos;
    Region* reg = GetRegion(reg_pos);

    for (const auto& offset : Directions<ChunkPos>()) {
        ChunkPos neighborPos = pos + offset;

        if (CheckChunk(neighborPos)) {
            GetChunk(neighborPos)->SetNeighbor(nullptr, !offset);
        }
    }

    // erase chunk from chunks_
    Chunk* swappedChunk = chunks_.back();

    int idx = chunks_idx_[pos];
    chunks_idx_[swappedChunk->position_] = idx;
    chunks_idx_.erase(pos);
    std::swap(chunks_[idx], chunks_.back());
    chunks_.pop_back();

    ChunkContainer* bottomChunk =
        reg->GetChunk(pos)->GetNeighbor(Directions<ChunkPos>::kDown);

    reg->EraseChunk(pos);

    if (heightmap_update_) {
        bottomChunk->UpdateHeightMap();
    }

    // Delete region if it is empty
    if (reg->GetChunkCount() == 0) {
        DeleteRegion(reg_pos);
    }
}

bool ChunkMap::SetBlock(BlockID block, const BlockPos& pos) {
    ChunkPos chunk_pos = pos;
    RegionPos reg_pos = pos;
    BlockPos local_pos = pos.GetLocalPos();

    Region* reg = GetRegionUncheck(reg_pos);
    if (reg == nullptr) return false;

    Chunk* chunk = reg->GetChunk(chunk_pos);

    if (chunk != nullptr) {
        chunk->SetBlockUnsafe(block, local_pos);
        chunk->is_empty_ = false;

        if (heightmap_update_) {
            chunk->UpdateHeightMap(local_pos.x, local_pos.z);
        }

        return true;
    }

    return false;
}

BlockID ChunkMap::GetBlock(const BlockPos& pos) const {
    ChunkPos chunk_pos = pos;
    RegionPos reg_pos = pos;
    BlockPos local_block_pos = pos.GetLocalPos();

    Region* reg = GetRegionUncheck(reg_pos);
    if (reg == nullptr) return g_blocks.AIR;

    if (!reg->CheckChunk(chunk_pos)) {
        return g_blocks.AIR;
    }

    Chunk* chunk = reg->GetChunk(chunk_pos);

    return chunk->GetBlock(local_block_pos);
}

bool ChunkMap::CheckChunk(const ChunkPos& pos) const {
    RegionPos reg_pos = pos;
    Region* reg = GetRegionUncheck(reg_pos);
    if (reg == nullptr) return false;
    return reg->CheckChunk(pos);
}

Chunk* ChunkMap::GetChunk(const ChunkPos& pos) const {
    RegionPos reg_pos = pos;
    Region* reg = GetRegionUncheck(reg_pos);
    if (reg == nullptr) return nullptr;

    return reg->GetChunk(pos);
}

std::vector<Chunk*> ChunkMap::GetAllChunks() const { return chunks_; }

void ChunkMap::InsertChunk(std::unique_ptr<Chunk> chunk) {
    const ChunkPos pos = chunk->position_;

    RegionPos reg_pos = pos;
    Region* reg = GetRegionUncheck(reg_pos);

    if (reg == nullptr) {
        reg = CreateRegion(reg_pos);
    }

    for (const auto& offset : Directions<ChunkPos>()) {
        ChunkPos posNeighbor = chunk->position_ + offset;

        if (CheckChunk(posNeighbor)) {
            chunk->SetNeighbor(
                static_cast<ChunkContainer*>(GetChunk(posNeighbor)), offset);
            GetChunk(posNeighbor)
                ->SetNeighbor(static_cast<ChunkContainer*>(chunk.get()),
                              !offset);
            if (neighbor_update_) {
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

    if (it == regions_.end()) {  // don't the cache because it is not in there
        g_logger.LogError("ChunkMap::GetRegion",
                          "Tried to get region that doesn't exist!");
    }
    Region* region = it->second.get();

    if (region_cache_.size() <= kRegionCacheSize) {
        region_cache_.emplace_back(pos, region);
        return region;
    }

    // Check if the region is less than the min
    if (minUsageCount <
        region->GetUsageCount()) {  // replace the min with the more used region
        region_cache_[minIdx] = {pos, region};
    }

    return region;
}

Region* ChunkMap::GetRegionUncheck(const RegionPos& pos) const {
    // First try to find region in the cache
    size_t minUsageCount = std::numeric_limits<size_t>::max();
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
    if (minUsageCount <
        region->GetUsageCount()) {  // replace the min with the more used region
        region_cache_[minIdx] = {pos, region};
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
            region_cache_[minIdx] = {pos, region};
        }

        return true;

    } else {
        return regions_.contains(pos);
    }
}

Region* ChunkMap::CreateRegion(const RegionPos& pos) {
    if (CheckRegion(pos)) {
        g_logger.LogError("ChunkMap::CreateRegion",
                          "Tried to create region when one already exist!");
    }
    std::unique_ptr<Region> newRegion = std::make_unique<Region>();
    Region* region = newRegion.get();
    regions_.emplace(pos, std::move(newRegion));
    return region;
}

void ChunkMap::DeleteRegion(const RegionPos& pos) {
    if (!CheckRegion(pos)) {
        g_logger.LogError("ChunkMap::DeleteRegion",
                          "Tried to delete region that is already deleted!");
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

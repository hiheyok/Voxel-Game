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

    if (!CheckRegion(regPos)) return false;

    Region* reg = GetRegion(regPos);

    Chunk* chunk = reg->GetChunk(chunkPos);

    if (chunk != nullptr) {
        chunk->Use();
        chunk->SetBlockUnsafe(block, localPos.x, localPos.y, localPos.z);
        chunk->is_empty_ = false;
        chunk->Unuse();

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

    if (!CheckRegion(regPos)) return g_blocks.AIR;

    Region* reg = GetRegion(regPos);

    if (!reg->CheckChunk(chunkPos)) {
        return g_blocks.AIR;
    }

    Chunk* chunk = reg->GetChunk(chunkPos);

    return chunk->GetBlock(localBlockPos.x, localBlockPos.y, localBlockPos.z);
}

bool ChunkMap::CheckChunk(const ChunkPos& pos) const {
    RegionPos regPos = pos / kRegionDim;
    if (!CheckRegion(regPos)) return g_blocks.AIR;
    Region* reg = GetRegion(regPos);

    return reg->CheckChunk(pos);
}

Chunk* ChunkMap::GetChunk(const ChunkPos& pos) const {
    RegionPos regPos = pos / kRegionDim;
    if (!CheckRegion(regPos)) return nullptr;
    Region* reg = GetRegion(regPos);

    return reg->GetChunk(pos);
}

std::vector<Chunk*> ChunkMap::GetAllChunks() const {
    return chunks_;
}

void ChunkMap::InsertChunk(std::unique_ptr<Chunk> chunk) {
    const ChunkPos pos = chunk->position_;

    RegionPos regPos = pos / kRegionDim;

    if (!CheckRegion(regPos)) {
        CreateRegion(regPos);
    }

    Region* reg = GetRegion(regPos);

    for (int axis_ = 0; axis_ < 3; axis_++) {
        for (int face = 0; face < 2; face++) {
            ChunkPos posNeighbor = chunk->position_;
            posNeighbor.incrementSide(axis_ * 2 + face, 1);

            if (CheckChunk(posNeighbor)) {
                chunk->SetNeighbor(static_cast<ChunkContainer*>(GetChunk(posNeighbor)), axis_ * 2 + face);
                GetChunk(posNeighbor)->SetNeighbor(static_cast<ChunkContainer*>(chunk.get()), axis_ * 2 - face + 1);
                GetChunk(posNeighbor)->Use();
                GetChunk(posNeighbor)->UpdateGen();
                GetChunk(posNeighbor)->Unuse();
            }
        }
    }

    chunk->Use();
    chunk->UpdateGen();
    chunk->Unuse();

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
    if (!CheckRegion(pos)) {
        g_logger.LogError("ChunkMap::GetRegion", "Tried to get region that doesn't exist!");
    }
    const auto& it = regions_.find(pos);
    return it->second.get();
}

bool ChunkMap::CheckRegion(const RegionPos& pos) const {
    return regions_.contains(pos);
}

void ChunkMap::CreateRegion(const RegionPos& pos) {
    if (CheckRegion(pos)) {
        g_logger.LogError("ChunkMap::CreateRegion", "Tried to create region when one already exist!");
    }
    std::unique_ptr<Region> newRegion = std::make_unique<Region>();
    regions_.emplace(pos, std::move(newRegion));
}

void ChunkMap::DeleteRegion(const RegionPos& pos) {
    if (!CheckRegion(pos)) {
        g_logger.LogError("ChunkMap::DeleteRegion", "Tried to delete region that is already deleted!");
    }

    regions_.erase(pos);
}


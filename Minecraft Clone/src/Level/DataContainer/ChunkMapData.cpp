#include "Level/DataContainer/ChunkMapData.h"
#include "Level/Chunk/Chunk.h"

bool ChunkMap::EraseChunk(const ChunkPos& pos) { 
    Region* reg = GetRegion(pos);

    for (int axis_ = 0; axis_ < 3; axis_++) {
        for (int face = 0; face < 2; face++) {
            ChunkPos posNeighbor = pos;
            posNeighbor.incrementSide(axis_ * 2 + face, 1);

            if (CheckChunk(posNeighbor)) {
                GetChunk(posNeighbor)->SetNeighbor(nullptr, axis_ * 2 - face + 1);
            }
        }
    }

    if (reg != nullptr) {
        reg->EraseChunk(pos.x, pos.y, pos.z);
        return true;
    }
    else {
        return false;
    }
}


bool ChunkMap::SetBlock(BlockID block, const BlockPos& pos) {
    ChunkPos chunkPos = pos / kChunkDim;
    BlockPos localPos = pos & (kChunkDim - 1);
    ChunkPos regionLocalPos = chunkPos & (kRegionDim - 1);

    if (!CheckRegion(chunkPos)) return false;

    Region* reg = GetRegion(chunkPos);


    Chunk* chunk = reg->GetChunk(regionLocalPos.x, regionLocalPos.y, regionLocalPos.z);

    if (chunk != nullptr) {
        chunk->Use();
        chunk->SetBlockUnsafe(block, localPos.x, localPos.y, localPos.z);
        chunk->UpdateHeightMap(localPos.x, localPos.z);
        chunk->is_empty_ = false;
        chunk->Unuse();
        return true;
    }

    return false;
}

BlockID ChunkMap::GetBlock(const BlockPos& pos) const {
    //c[3] is the position of the chunk
    //l[3] is the local position of a block inside a chunk

    ChunkPos chunkPos = pos / kChunkDim;
    BlockPos localBlockPos = pos & (kChunkDim - 1);

    if (!CheckRegion(chunkPos)) return g_blocks.AIR;


    Region* reg = GetRegion(chunkPos);

    Chunk* chunk = reg->GetChunk(chunkPos.x, chunkPos.y, chunkPos.z);

    if (chunk == nullptr) {
        return g_blocks.AIR;
    }

    BlockID block = chunk->GetBlock(localBlockPos.x, localBlockPos.y, localBlockPos.z);

    return block;
}

bool ChunkMap::CheckChunk(const ChunkPos& pos) const {
    if (!CheckRegion(pos)) return g_blocks.AIR;
    Region* reg = GetRegion(pos);

    return reg->CheckChunk(pos.x, pos.y, pos.z);
}



Chunk* ChunkMap::GetChunk(const ChunkPos& pos) const {
    if (!CheckRegion(pos)) return nullptr;
    Region* reg = GetRegion(pos);

    return reg->GetChunk(pos.x, pos.y, pos.z);
}

void ChunkMap::InsertChunk(std::unique_ptr<Chunk> chunk) {
    const ChunkPos& pos = chunk->position_;
    
    int x = pos.x;
    int y = pos.y;
    int z = pos.z;

    RegionPos regionPos = pos >> 5;

    if (!CheckRegion(pos)) {
        live_region_[regionPos] = std::make_unique<Region>();
    }

    Region* reg = GetRegion(pos);

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

    // Update heightmaps
    chunk->UpdateHeightMap();

    if (chunk->neighbors_[NY] != nullptr) {
        chunk->neighbors_[NY]->UpdateHeightMap();
    }

    if (chunk->neighbors_[PY] != nullptr) {
        chunk->neighbors_[PY]->UpdateHeightMap();
    }

    reg->AddChunk(std::move(chunk));
}

bool ChunkMap::CheckRegion(const ChunkPos& pos) const {
    RegionPos regPos = pos >> 5;

    FastHashMap<RegionPos, std::unique_ptr<Region>>::const_iterator it = live_region_.find(regPos);

    if (it == live_region_.end()) {
        return false;
    }
    else {
        return true;
    }
}

Region* ChunkMap::GetRegion(const ChunkPos& pos) const {
    RegionPos regPos = pos >> 5;

    FastHashMap<RegionPos, std::unique_ptr<Region>>::const_iterator it = live_region_.find(regPos);

    if (it == live_region_.end()) {
        throw std::runtime_error("Tried to get invalid region");
    }

    return it->second.get();
}
//_____________________________________________________

Region::Region() {
    region_.resize(kRegionSize3D);
}

Region::~Region() {
    region_.clear();
}

void Region::AddChunk(std::unique_ptr<Chunk> chunk) {
    int x = chunk->position_.x & (kRegionDim - 1);
    int y = chunk->position_.y & (kRegionDim - 1);
    int z = chunk->position_.z & (kRegionDim - 1);

    region_[x * kRegionSize2D + y * kRegionDim + z] = std::move(chunk);
}

void Region::EraseChunk(int x, int y, int z) {
    x = x & (kRegionDim - 1);
    y = y & (kRegionDim - 1);
    z = z & (kRegionDim - 1);

    region_[x * kRegionSize2D + y * kRegionDim + z] = nullptr;
}

bool Region::CheckChunk(int x, int y, int z) const {
    x = x & (kRegionDim - 1);
    y = y & (kRegionDim - 1);
    z = z & (kRegionDim - 1);

    return region_[x * kRegionSize2D + y * kRegionDim + z] != nullptr;
}

Chunk* Region::GetChunk(int x, int y, int z) const {
    x = x & (kRegionDim - 1);
    y = y & (kRegionDim - 1);
    z = z & (kRegionDim - 1);

    return region_[x * kRegionSize2D + y * kRegionDim + z].get();
}
#include "ChunkMapData.h"


bool ChunkMap::EraseChunk(const ChunkPos& pos) { 
    const std::unique_ptr<Region>& reg = GetRegion(pos);

    // TODO: Fix me

    if (reg != nullptr) {
        reg->EraseChunkGlobalPos(pos.x, pos.y, pos.z);
        return true;
    }
    return false;
}


bool ChunkMap::SetBlockGlobal(BlockID block, const BlockPos& pos) {
    // TODO: Make it so BlockPos has a method to convert to chunk position location
    ChunkPos chunkPos = pos >> 4;
    BlockPos localPos = pos & 0b1111;

    if (!CheckRegion(chunkPos)) return false;

    const std::unique_ptr<Region>& reg = GetRegion(chunkPos);

    ChunkPos regionLocalPos = chunkPos & 0b11111;

    Chunk* chunk = reg->GetChunk(regionLocalPos.x, regionLocalPos.y, regionLocalPos.z);

    if (chunk != nullptr) {
        chunk->Use();
        chunk->SetBlockUnsafe(block, localPos.x, localPos.y, localPos.z);
        chunk->is_empty_ = false;
        chunk->Unuse();

        //Set lighting update to dirty
            
        ChunkColumn* col = reg->GetChunkColumn(regionLocalPos.x, regionLocalPos.z);
        col->UpdateHeightmapSingleBlock(regionLocalPos.y, block, localPos.z, localPos.y, localPos.z);

        return true;
    }

    return false;
}

bool ChunkMap::CheckChunk(const ChunkPos& pos) const {
    if (!CheckRegion(pos)) return g_blocks.AIR;
    const std::unique_ptr<Region>& reg = GetRegion(pos);

    return reg->CheckChunkGlobalPos(pos.x, pos.y, pos.z);
}

BlockID ChunkMap::GetBlockGlobal(const BlockPos& pos) {
    //c[3] is the position of the chunk
    //l[3] is the local position of a block inside a chunk

    int x = pos.x;
    int y = pos.y;
    int z = pos.z;

    ChunkPos chunkPos = pos >> 4;

    if (!CheckRegion(chunkPos)) return g_blocks.AIR;

    BlockPos localBlockPos = pos & 0b1111;

    const std::unique_ptr<Region>& reg = GetRegion(chunkPos);

    // TODO: Fix me
    Chunk* chunk = reg->GetChunkGlobalPos(chunkPos.x, chunkPos.y, chunkPos.z);

    if (chunk == nullptr) {
        return g_blocks.AIR;
    }

    BlockID block = chunk->GetBlock(localBlockPos.x, localBlockPos.y, localBlockPos.z);

    return block;
}

Chunk* ChunkMap::GetChunk(const ChunkPos& pos) const {
    if (!CheckRegion(pos)) return nullptr;
    const std::unique_ptr<Region>& reg = GetRegion(pos);

    return reg->GetChunkGlobalPos(pos.x, pos.y, pos.z);
}

ChunkColumn* ChunkMap::GetColumn(const ChunkColumnPos& pos) const {
    if (!CheckRegion(pos)) return nullptr;
    const std::unique_ptr<Region>& reg = GetRegion(pos);

    return reg->GetChunkColumnGlobalPos(pos.x, pos.z);
}

void ChunkMap::InsertChunk(Chunk* chunk) {
    const ChunkPos& pos = chunk->position_;
    
    int x = pos.x;
    int y = pos.y;
    int z = pos.z;

    RegionPos regionPos = pos >> 5;

    if (!CheckRegion(pos)) {
        live_region_[regionPos] = std::make_unique<Region>();
    }

    const std::unique_ptr<Region>& reg = GetRegion(pos);

    reg->AddChunkGlobalPos(chunk, x, y, z);

    for (int axis_ = 0; axis_ < 3; axis_++) {
        for (int face = 0; face < 2; face++) {
            ChunkPos posNeighbor = chunk->position_;
            posNeighbor.incrementSide(axis_ * 2 + face, 1);
            
            if (CheckChunk(posNeighbor)) {
                chunk->SetNeighbor(static_cast<ChunkContainer*>(GetChunk(posNeighbor)), axis_ * 2 + face);
                GetChunk(posNeighbor)->SetNeighbor(static_cast<ChunkContainer*>(chunk), axis_ * 2 - face + 1);
                GetChunk(posNeighbor)->Use();
                GetChunk(posNeighbor)->UpdateGen();
                GetChunk(posNeighbor)->Unuse();
            }
        }
    }
    chunk->Use();
    chunk->UpdateGen();
    chunk->Unuse();
}

const bool ChunkMap::CheckRegion(const ChunkPos& pos) const {
    // TODO: Use a different class for region pos 
    RegionPos regPos = pos >> 5;

    FastHashMap<RegionPos, std::unique_ptr<Region>>::const_iterator it = live_region_.find(regPos);

    if (it == live_region_.end()) {
        return false;
    }
    else {
        return true;
    }
}

const std::unique_ptr<Region>& ChunkMap::GetRegion(const ChunkPos& pos) const {
    // TODO: Use a different class for region pos 
    RegionPos regPos = pos >> 5;

    FastHashMap<RegionPos, std::unique_ptr<Region>>::const_iterator it = live_region_.find(regPos);

    if (it == live_region_.end()) {
        throw std::runtime_error("Tried to get invalid region");
    }

    return it->second;
}
//_____________________________________________________

void Region::AddChunk(Chunk* chunk, uint16_t x, uint16_t y, uint16_t z) {
    if (region_[x * 32 + z] == nullptr) {
        region_[x * 32 + z] = new ChunkColumn();
    }
    region_[x * 32 + z]->AddChunk(chunk, y);
}

void Region::EraseChunk(uint16_t x, uint16_t y, uint16_t z) {
    ChunkColumn* c = region_[x * 32 + z];
    if (c == nullptr) return;
    if (c->GetChunk(y) == nullptr) return;

    c->GetChunk(y)->ClearNeighbors();

    delete c->GetChunk(y);
    region_[x * 32 + z] = nullptr;
}

bool Region::CheckChunk(uint16_t x, uint16_t y, uint16_t z) const {
    if (!CheckChunkColumn(x, z)) {
        return false;
    }
    return region_[x * 32 + z]->GetChunk(y) != nullptr;
}

Chunk* Region::GetChunk(uint16_t x, uint16_t y, uint16_t z) const {
    if (region_[x * 32 + z] != nullptr) {
        return region_[x * 32 + z]->GetChunk(y);
    }
    return nullptr;
}

void Region::AddChunkGlobalPos(Chunk* chunk, int32_t x, int32_t y, int32_t z) {
    uint16_t regCX = x & 0b11111;
    uint16_t regCY = y & 0b11111;
    uint16_t regCZ = z & 0b11111;

    AddChunk(chunk, regCX, regCY, regCZ);
}

void Region::EraseChunkGlobalPos(int32_t x, int32_t y, int32_t z) {
    uint16_t regCX = x & 0b11111;
    uint16_t regCY = y & 0b11111;
    uint16_t regCZ = z & 0b11111;

    EraseChunk(regCX, regCY, regCZ);
}

bool Region::CheckChunkGlobalPos(int32_t x, int32_t y, int32_t z) const {
    uint16_t regCX = x & 0b11111;
    uint16_t regCY = y & 0b11111;
    uint16_t regCZ = z & 0b11111;

    return CheckChunk(regCX, regCY, regCZ);
}

Chunk* Region::GetChunkGlobalPos(int32_t x, int32_t y, int32_t z) const {
    uint16_t regCX = x & 0b11111;
    uint16_t regCY = y & 0b11111;
    uint16_t regCZ = z & 0b11111;

    return GetChunk(regCX, regCY, regCZ);
}

ChunkColumn* Region::GetChunkColumn(uint16_t x, uint16_t z) const {
    return region_[x * 32 + z];
}

ChunkColumn* Region::GetChunkColumnGlobalPos(int32_t x, int32_t z) const {
    uint16_t regCX = x & 0b11111;
    uint16_t regCZ = z & 0b11111;

    return GetChunkColumn(regCX, regCZ);
}

bool Region::CheckChunkColumn(uint16_t x, uint16_t z) const {
    return region_[x * 32 + z] != nullptr;
}
bool Region::CheckChunkColumnGlobalPos(int32_t x, int32_t z) const {
    uint16_t regCX = x & 0b11111;
    uint16_t regCZ = z & 0b11111;

    return CheckChunkColumn(regCX, regCZ);
}
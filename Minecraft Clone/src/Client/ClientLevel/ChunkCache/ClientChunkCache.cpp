#include "Client/ClientLevel/ChunkCache/ClientChunkCache.h"
#include "Client/ClientLevel/ChunkCache/ClientRegion.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/ChunkRawData.h"

ClientChunkCache::ClientChunkCache() = default;
ClientChunkCache::~ClientChunkCache() = default;

void ClientChunkCache::EraseChunk(const ChunkPos& pos) {
    RegionPos regPos = pos / kRegionDim;
    ClientRegion* reg = GetRegion(regPos);
    if (reg != nullptr) {
        reg->EraseChunk(pos);
    }
    
    // Delete region if it is empty
    if (reg->GetChunkCount() == 0) {
        DeleteRegion(regPos);
    }
}


void ClientChunkCache::SetBlock(BlockID block, const BlockPos& pos) {
    ChunkPos chunkPos = pos / kChunkDim;
    BlockPos localPos = pos & (kChunkDim - 1);
    RegionPos regPos = chunkPos / kRegionDim;

    if (!CheckRegion(regPos)) return;

    ClientRegion* reg = GetRegion(regPos);

    Chunk* chunk = reg->GetChunk(chunkPos);

    if (chunk != nullptr) {
        chunk->Use();
        chunk->SetBlockUnsafe(block, localPos.x, localPos.y, localPos.z);
        chunk->is_empty_ = false;
        chunk->Unuse();
    }
}

bool ClientChunkCache::CheckChunk(const ChunkPos& pos) const {
    RegionPos regPos = pos / kRegionDim;
    if (!CheckRegion(regPos)) return g_blocks.AIR;
    ClientRegion* reg = GetRegion(regPos);

    return reg->CheckChunk(pos);
}

BlockID ClientChunkCache::GetBlock(const BlockPos& pos) const {
    ChunkPos chunkPos = pos / kChunkDim;
    RegionPos regPos = chunkPos / kRegionDim;

    if (!CheckRegion(regPos)) return g_blocks.AIR;

    BlockPos localBlockPos = pos & (kChunkDim - 1);

    ClientRegion* reg = GetRegion(regPos);

    if (!reg->CheckChunk(chunkPos)) {
        return g_blocks.AIR;
    }

    Chunk* chunk = reg->GetChunk(chunkPos);

    return chunk->GetBlock(localBlockPos.x, localBlockPos.y, localBlockPos.z);
}

Chunk* ClientChunkCache::GetChunk(const ChunkPos& pos) const {
    RegionPos regPos = pos / kRegionDim;
    if (!CheckRegion(regPos)) return nullptr;
    ClientRegion* reg = GetRegion(regPos);

    return reg->GetChunk(pos);
}

void ClientChunkCache::InsertChunk(std::unique_ptr<Chunk> chunk) {
    const ChunkPos& pos = chunk->position_;

    int x = pos.x;
    int y = pos.y;
    int z = pos.z;

    RegionPos regPos = pos / kRegionDim;

    if (!CheckRegion(regPos)) {
        CreateRegion(regPos);
    }

    ClientRegion* reg = GetRegion(regPos);

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
        reg->InsertChunk(std::move(chunk));
    }
}

ClientRegion* ClientChunkCache::GetRegion(const RegionPos& pos) const {
    if (!CheckRegion(pos)) {
        g_logger.LogError("ClientChunkCache::GetRegion", "Tried to get region that doesn't exist!");
    }
    const auto& it = regions_.find(pos);
    return it->second.get();
}

bool ClientChunkCache::CheckRegion(const RegionPos& pos) const {
    return regions_.contains(pos);
}

void ClientChunkCache::CreateRegion(const RegionPos& pos) {
    if (CheckRegion(pos)) {
        g_logger.LogError("ClientChunkCache::CreateRegion", "Tried to create region when one already exist!");
    }
    std::unique_ptr<ClientRegion> newRegion = std::make_unique<ClientRegion>();
    regions_.emplace(pos, std::move(newRegion));
}

void ClientChunkCache::DeleteRegion(const RegionPos& pos) {
    if (!CheckRegion(pos)) {
        g_logger.LogError("ClientChunkCache::DeleteRegion", "Tried to delete region that is already deleted!");
    }

    regions_.erase(pos);
}


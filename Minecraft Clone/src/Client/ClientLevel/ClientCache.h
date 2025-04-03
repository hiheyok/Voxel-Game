#pragma once
#include "Level/Container/ChunkMap.h"
#include "Level/Container/Region.h"
#include "Level/Chunk/Chunk.h"
#include "Level/World/Collusion/CollusionDetector.h"
#include "Client/ClientLevel/Entity/ClientEntities.h"

class ClientCache {
public:
    ClientEntities entities_;
    CollusionDetector collusion_manager_;

    ClientCache() : collusion_manager_{&chunk_cache_ } {}

    void AddChunk(std::unique_ptr<Chunk> chunk) {
        chunk_cache_.InsertChunk(std::move(chunk));
    }

    void EraseChunk(const ChunkPos& pos) {
        chunk_cache_.EraseChunk(pos);
    }

    Chunk* GetChunk(const ChunkPos& pos) const {
        return chunk_cache_.GetChunk(pos);
    }

    BlockID GetBlock(const BlockPos& pos) const {
        return chunk_cache_.GetBlock(pos);
    }
    
    bool CheckChunk(const ChunkPos& pos) const {
        return chunk_cache_.CheckChunk(pos);
    }
private:
    ChunkMap chunk_cache_;
};
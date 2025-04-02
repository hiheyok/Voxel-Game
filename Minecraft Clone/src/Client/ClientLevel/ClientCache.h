#pragma once
#include "Client/ClientLevel/ChunkCache/ClientChunkCache.h"
#include "Client/ClientLevel/ChunkCache/ClientCollusionDetector.h"
#include "Client/ClientLevel/Entity/ClientEntities.h"
#include "Level/Chunk/Chunk.h"

class ClientCache {
private:
    ClientChunkCache chunk_cache_;
public:
    ClientEntities entities_;
    ClientCollusionDetector collusion_manager_;

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
};
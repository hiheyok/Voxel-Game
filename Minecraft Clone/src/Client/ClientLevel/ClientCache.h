#pragma once
#include "../../Level/DataContainer/ChunkMapData.h"
#include "Entity/ClientEntities.h"
#include "../../Level/World/Collusion/WorldCollusion.h"

class ClientCache {
private:
    ChunkMap chunk_cache_;
public:
    ClientEntities entities_;
    WorldCollusionDetector collusion_manager_;

    ClientCache() {
        collusion_manager_.Initialize(&chunk_cache_);
    }

    void AddChunk(std::unique_ptr<Chunk> chunk) {
        chunk_cache_.InsertChunk(std::move(chunk));
    }

    void EraseChunk(ChunkPos pos) {
        chunk_cache_.EraseChunk(pos);
    }

    Chunk* GetChunk(ChunkPos pos) const {
        return chunk_cache_.GetChunk(pos);
    }
};
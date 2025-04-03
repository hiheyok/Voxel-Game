#pragma once
#include <memory>
#include <vector>

#include "Core/Typenames.h"

class Chunk;

/*
* Simple chunk storage for the client
*/

class Region {
public:
    Region();
    ~Region();

    Chunk* GetChunk(const ChunkPos& pos) const;
    void InsertChunk(std::unique_ptr<Chunk> chunk);
    void EraseChunk(const ChunkPos& pos);
    bool CheckChunk(const ChunkPos& pos) const;
    int GetChunkCount() const;

private:
    int chunk_count_;
    std::vector<std::unique_ptr<Chunk>> region_data_;
};
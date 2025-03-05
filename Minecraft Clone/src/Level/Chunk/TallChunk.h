#pragma once
#include "Chunk.h"

class TallChunk {
private:
    std::vector<Chunk*> chunk_sub_column_;
public:
    TallChunk() {
        chunk_sub_column_.resize(16);
        for (int i = 0; i < 16; i++) chunk_sub_column_[i] = new Chunk;
    }

    void SetPosition(int x, int y, int z) { // confusing y -> tall chunk pos
        y = y * 16;
        for (int i = 0; i < 16; i++) {
            chunk_sub_column_[i]->SetPosition(x, y + i, z);
        }
    }

    int GetYPosition() {
        return chunk_sub_column_[0]->position_.y;
    }

    int GetXPosition() {
        return chunk_sub_column_[0]->position_.x;
    }

    int GetZPosition() {
        return chunk_sub_column_[0]->position_.z;
    }

    void SetBlockUnsafe(int x, int y, int z, BlockID block) {
        int ChunkIndex = y / 16;
        chunk_sub_column_[ChunkIndex]->SetBlockUnsafe(block, x, y & 0b1111, z );
    }

    BlockID GetBlockUnsafe(int x, int y, int z) {
        int ChunkIndex = y / 16;
        return chunk_sub_column_[ChunkIndex]->GetBlockUnsafe(x, y & 0b1111, z );
    }

    std::vector<Chunk*> GetCubicChunks() {
        return chunk_sub_column_;
    }
};
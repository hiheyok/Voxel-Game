#include "Level/Chunk/TallChunk.h"

TallChunk::TallChunk() {
    chunk_sub_column_.resize(16);
    for (int i = 0; i < 16; i++) chunk_sub_column_[i] = std::make_unique<Chunk>();
}

TallChunk::~TallChunk() = default;

void TallChunk::SetPosition(int x, int y, int z) { // confusing y -> tall chunk pos
    y = y * 16;
    for (int i = 0; i < 16; i++) {
        chunk_sub_column_[i]->SetPosition(ChunkPos{x, y + i, z});
    }
}

int TallChunk::GetYPosition() {
    return chunk_sub_column_[0]->position_.y;
}

int TallChunk::GetXPosition() {
    return chunk_sub_column_[0]->position_.x;
}

int TallChunk::GetZPosition() {
    return chunk_sub_column_[0]->position_.z;
}

// TODO: Refactor to use TallBlockPos later
void TallChunk::SetBlockUnsafe(int x, int y, int z, BlockID block) {
    int ChunkIndex = y / 16;
    chunk_sub_column_[ChunkIndex]->SetBlockUnsafe(block, BlockPos{x, y & 0b1111, z});
}

BlockID TallChunk::GetBlockUnsafe(int x, int y, int z) {
    int ChunkIndex = y / 16;
    return chunk_sub_column_[ChunkIndex]->GetBlockUnsafe(BlockPos{x, y & 0b1111, z});
}

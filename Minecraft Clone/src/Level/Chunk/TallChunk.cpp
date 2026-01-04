// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Chunk/TallChunk.h"

#include <memory>

#include "Core/Typenames.h"
#include "Level/Block/Block.h"
#include "Level/Chunk/Chunk.h"

using std::make_unique;

TallChunk::TallChunk(GameContext& context) : context_{context} {
  chunk_sub_column_.resize(16);
  for (int i = 0; i < 16; i++)
    chunk_sub_column_[i] = make_unique<Chunk>(context);
}

TallChunk::~TallChunk() = default;

void TallChunk::SetPosition(int x, int y,
                            int z) {  // confusing y -> tall chunk pos
  y = y * 16;
  for (int i = 0; i < 16; i++) {
    chunk_sub_column_[i]->SetPosition({x, y + i, z});
  }
}

int TallChunk::GetYPosition() { return chunk_sub_column_[0]->position_.y; }

int TallChunk::GetXPosition() { return chunk_sub_column_[0]->position_.x; }

int TallChunk::GetZPosition() { return chunk_sub_column_[0]->position_.z; }

// TODO(hiheyok): Refactor to use TallBlockPos later
void TallChunk::SetBlockUnsafe(int x, int y, int z, BlockID block) {
  int ChunkIndex = y / 16;
  chunk_sub_column_[ChunkIndex]->SetBlockUnsafe(block, {x, y & 0b1111, z});
}

BlockID TallChunk::GetBlockUnsafe(int x, int y, int z) {
  int ChunkIndex = y / 16;
  return chunk_sub_column_[ChunkIndex]->GetBlockUnsafe({x, y & 0b1111, z});
}

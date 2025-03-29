#include "ChunkData.h"

static const int NeighborOffset[6] = {
      -16, 16,-16, 16,-16, 16
};

// TODO: Make this readable and safe
BlockID ChunkContainer::GetBlock(int x, int y, int z) const {
    if (!((x | y | z) >> 4)) { //check if it is in the chunk
        return block_storage_.GetBlock(x, y, z);
    }

    int dx = ((x >> 31) & 0b1) + 0;
    int dy = ((y >> 31) & 0b1) + 2;
    int dz = ((z >> 31) & 0b1) + 4;

    if (neighbors_[dx] && (x >> 4)) return neighbors_[dx]->GetBlock(x + NeighborOffset[dx], y, z );
    if (neighbors_[dy] && (y >> 4)) return neighbors_[dy]->GetBlock(x, y + NeighborOffset[dy], z );
    if (neighbors_[dz] && (z >> 4)) return neighbors_[dz]->GetBlock(x, y, z + NeighborOffset[dz]);

    return g_blocks.AIR;
}

BlockID ChunkContainer::GetBlockUnsafe(int x, int y, int z) const {
    return block_storage_.GetBlockUnsafe(x, y, z);
}

//TODO: debug this later (trees getting cut off at chunk borders)
void ChunkContainer::SetBlock(BlockID block, int x, int y, int z) {
    if (!((x | y | z) >> 4)) { //check if it is in the chunk
        SetBlockUnsafe(block, x, y, z);
        return;
    }

    int dx = ((x >> 31) & 0b1) + 0;
    int dy = ((y >> 31) & 0b1) + 2;
    int dz = ((z >> 31) & 0b1) + 4;

    if (neighbors_[dx] && (x >> 4)) {
        outside_block_to_place_[dx].emplace_back(block, x + NeighborOffset[dx], y, z);
        return;
    }
    if (neighbors_[dy] && (y >> 4)) {
        outside_block_to_place_[dy].emplace_back(block, x, y + NeighborOffset[dy], z);
        return;
    }
    if (neighbors_[dz] && (z >> 4)) {
        outside_block_to_place_[dz].emplace_back(block, x, y, z + NeighborOffset[dz]);
        return;
    }
}

void ChunkContainer::SetBlockUnsafe(BlockID block, int x, int y, int z) {
    block_storage_.SetBlockUnsafe(block, (uint32_t)x, (uint32_t)y, (uint32_t)z);

    is_empty_ = false;
}

void ChunkContainer::SetPosition(int x, int y, int z) {
    position_.set(x, y, z);
}

ChunkContainer* ChunkContainer::GetNeighbor(unsigned int side) const {
    return neighbors_[side];
}

void ChunkContainer::SetNeighbor(ChunkContainer* Neighbor, unsigned int Side) {
    neighbors_[Side] = Neighbor;
}

void ChunkContainer::ClearNeighbors() {
    for (int i = 0; i < 6; i++) {
        neighbors_[i] = nullptr;
    }
}

void ChunkContainer::Use() {
    while (in_use_) {};
    in_use_ = true;
}

void ChunkContainer::Unuse() {
    in_use_ = false;
}
#include "Level/Chunk/Chunk.h"

Chunk::Chunk() = default;
Chunk::~Chunk() = default;
Chunk::Chunk(const ChunkRawData& data) : ChunkContainer{ data } {}

void Chunk::UpdateGen() {
    for (int side = 0; side < 6; side++) {
        int axis_ = side >> 1;
        int face = side & 0b1;

        if (neighbors_[side] == nullptr)
            continue;

        if (neighbors_[side]->outside_block_to_place_[axis_ * 2 + (!face)].size() == 0) {
            continue;
        }

        for (const auto& block : neighbors_[side]->outside_block_to_place_[axis_ * 2 + (!face)]) {
            SetBlock(block.block_, block.x_, block.y_, block.z_);
        }

        neighbors_[side]->outside_block_to_place_[axis_ * 2 + (!face)].clear();
    }
}
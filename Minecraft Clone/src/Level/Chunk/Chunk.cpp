#include "Chunk.h"

Chunk::Chunk() = default;
Chunk::~Chunk() = default;

void Chunk::UpdateGen() {
    for (int side = 0; side < 6; side++) {
        int axis_ = side >> 1;
        int face = side & 0b1;

        if (neighbors_[side] == nullptr)
            continue;

        if (neighbors_[side]->outside_block_to_place_[axis_ * 2 + (!face)].size() == 0) {
            continue;
        }

        std::vector<SetBlockRelative> blocks = neighbors_[side]->outside_block_to_place_[axis_ * 2 + (!face)];

        neighbors_[side]->outside_block_to_place_[axis_ * 2 + (!face)].clear();

        for (int i = 0; i < blocks.size(); i++) {
            SetBlock(blocks[i].block_, blocks[i].x_, blocks[i].y_, blocks[i].z_);
        }

        blocks.clear();
    }
}
#include "Level/Chunk/Chunk.h"
#include "Level/Light/LightStorage.h"
#include "Level/TerrainGeneration/Structures/Structure.h"

Chunk::Chunk() = default;
Chunk::~Chunk() = default;
Chunk::Chunk(const ChunkRawData& data) : ChunkContainer{ data } {}

void Chunk::UpdateGen() {
    for (const auto& offset : Directions()) {
        if (neighbors_[offset] == nullptr || 
            neighbors_[offset]->outside_block_to_place_[!offset].size() == 0) {
            continue;
        }

        for (const auto& block : neighbors_[offset]->outside_block_to_place_[!offset]) {
            SetBlock(block.block_, block.x_, block.y_, block.z_);
        }

        neighbors_[offset]->outside_block_to_place_[!offset].clear();
    }
}
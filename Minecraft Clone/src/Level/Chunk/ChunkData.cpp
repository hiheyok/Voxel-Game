#include "Level/Chunk/ChunkData.h"
#include "Level/Chunk/ChunkRawData.h"
#include "Level/Chunk/Heightmap/Heightmap.h"
#include "Level/TerrainGeneration/Structures/Structure.h"

static const int NeighborOffset[6] = {
      -16, 16,-16, 16,-16, 16
};

ChunkContainer::ChunkContainer() : 
    lighting_{ std::make_unique<LightStorage>() },
    neighbors_{ 6 },
    outside_block_to_place_{ 6 },
    heightmap_{ std::make_unique<Heightmap>() } {
    lighting_->position_ = position_;
}

ChunkContainer::~ChunkContainer() = default;
ChunkContainer::ChunkContainer(ChunkContainer&&) = default;

ChunkContainer::ChunkContainer(const ChunkRawData& data) : ChunkContainer() {
    SetData(data);
}

BlockID ChunkContainer::GetBlock(int x, int y, int z) const {
    if (!((x | y | z) >> 4)) {
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

void ChunkContainer::SetBlock(BlockID block, int x, int y, int z) {
    if (!((x | y | z) >> 4)) { //check if it is in the chunk
        SetBlockUnsafe(block, x, y, z);
        return;
    }

    int dx = ((x >> 31) & 0b1) + 0;
    int dy = ((y >> 31) & 0b1) + 2;
    int dz = ((z >> 31) & 0b1) + 4;

    if (x >> 4) {
        outside_block_to_place_[dx].emplace_back(block, x + NeighborOffset[dx], y, z);
        return;
    }
    if (y >> 4) {
        outside_block_to_place_[dy].emplace_back(block, x, y + NeighborOffset[dy], z);
        return;
    }
    if (z >> 4) {
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
    lighting_->position_.set(x, y, z);
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

void ChunkContainer::SetData(const ChunkRawData& data) {
    lighting_->ReplaceData(data.lighting_data_.GetData());
    block_storage_ = data.chunk_data_;
    position_ = data.pos_;
}

ChunkRawData ChunkContainer::GetRawData() {
    return ChunkRawData{ block_storage_, *lighting_.get(), position_ };
}

LightStorage ChunkContainer::GetLightData() {
    return LightStorage{ *lighting_.get() };
}

void ChunkContainer::UpdateHeightMap() {
    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            UpdateHeightMap(x, z);
        }
    }
}

void ChunkContainer::UpdateHeightMap(int x, int z) {
    // Check chunk above first, if the heightmap above is > -1, it means that there are block above
    // -1 indicate theirs nothing in the column
    ChunkContainer* chunkAbove = neighbors_[PY];

    int newHeight = -1; // -1 is the default height if there is no blocks in the column
    int oldHeight = heightmap_->Get(x, z);
    
    if (neighbors_[PY] != nullptr && neighbors_[PY]->heightmap_->Get(x, z) != -1) {
        newHeight = kChunkDim;
    } else {
        for (int i = 15; i >= 0; --i) {
            if (GetBlockUnsafe(x, i, z) != g_blocks.AIR) {
                newHeight = i;
                break;
            }
        }
    }

    if (newHeight != oldHeight) {
        light_dirty_ = true;
        heightmap_->Edit(x, z, newHeight);
    
        ChunkContainer* chunkBottom = neighbors_[NY];
        if (chunkBottom != nullptr && (oldHeight == -1 || newHeight == -1)) {
            chunkBottom->UpdateHeightMap(x, z);
        }
    }


}

bool ChunkContainer::CheckLightDirty() {
    bool isDirty = light_dirty_;
    light_dirty_ = false;
    return isDirty;
}
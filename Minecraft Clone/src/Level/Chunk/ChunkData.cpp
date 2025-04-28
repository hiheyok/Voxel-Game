#include "Level/Chunk/ChunkData.h"

#include "Level/Chunk/ChunkRawData.h"
#include "Level/Chunk/Heightmap/Heightmap.h"
#include "Level/TerrainGeneration/Structures/Structure.h"

static constexpr int NeighborOffset[6] = {-kChunkDim, kChunkDim,  -kChunkDim,
                                          kChunkDim,  -kChunkDim, kChunkDim};

ChunkContainer::ChunkContainer()
    : lighting_{std::make_unique<LightStorage>()},
      neighbors_{6},
      outside_block_to_place_{6},
      heightmap_{std::make_unique<Heightmap>()} {
    lighting_->position_ = position_;
}

ChunkContainer::~ChunkContainer() = default;
ChunkContainer::ChunkContainer(ChunkContainer&&) = default;

ChunkContainer::ChunkContainer(const ChunkRawData& data) : ChunkContainer() {
    SetData(data);
}

BlockID ChunkContainer::GetBlock(const BlockPos& pos) const {
    if ((pos.x | pos.y | pos.z) >> kChunkDimLog2) {
        int dx = ((pos.x >> 31) & 1) + 0;
        int dy = ((pos.y >> 31) & 1) + 2;
        int dz = ((pos.z >> 31) & 1) + 4;

        if (neighbors_[dx] && (pos.x >> kChunkDimLog2))
            return neighbors_[dx]->GetBlock(
                BlockPos{pos.x + NeighborOffset[dx], pos.y, pos.z});
        if (neighbors_[dy] && (pos.y >> kChunkDimLog2))
            return neighbors_[dy]->GetBlock(
                BlockPos{pos.x, pos.y + NeighborOffset[dy], pos.z});
        if (neighbors_[dz] && (pos.z >> kChunkDimLog2))
            return neighbors_[dz]->GetBlock(
                BlockPos{pos.x, pos.y, pos.z + NeighborOffset[dz]});

        return g_blocks.AIR;
    } else {
        return block_storage_.GetBlock(pos);
    }
}

BlockID ChunkContainer::GetBlockUnsafe(const BlockPos& pos) const {
    return block_storage_.GetBlockUnsafe(pos);
}

void ChunkContainer::SetBlock(BlockID block, const BlockPos& pos) {
    if ((pos.x | pos.y | pos.z) >>
        kChunkDimLog2) {  // check if it is in the chunk
        int dx = ((pos.x >> 31) & 1) + 0;
        int dy = ((pos.y >> 31) & 1) + 2;
        int dz = ((pos.z >> 31) & 1) + 4;

        if (pos.x >> kChunkDimLog2) {
            outside_block_to_place_[dx].emplace_back(
                block, BlockPos{pos.x + NeighborOffset[dx], pos.y, pos.z});
            return;
        }
        if (pos.y >> kChunkDimLog2) {
            outside_block_to_place_[dy].emplace_back(
                block, BlockPos{pos.x, pos.y + NeighborOffset[dy], pos.z});
            return;
        }
        if (pos.z >> kChunkDimLog2) {
            outside_block_to_place_[dz].emplace_back(
                block, BlockPos{pos.x, pos.y, pos.z + NeighborOffset[dz]});
            return;
        }
    } else {
        SetBlockUnsafe(block, pos);
    }
}

void ChunkContainer::SetBlockUnsafe(BlockID block, const BlockPos& pos) {
    block_storage_.SetBlockUnsafe(block, pos);
    is_empty_ = false;
}

void ChunkContainer::SetPosition(const ChunkPos& pos) {
    position_ = pos;
    lighting_->position_ = pos;
}

ChunkContainer* ChunkContainer::GetNeighbor(unsigned int side) const {
    return neighbors_[side];
}

void ChunkContainer::SetNeighbor(ChunkContainer* neighbor, unsigned int side) {
    neighbors_[side] = neighbor;
}

void ChunkContainer::ClearNeighbors() {
    for (const auto& side : Directions<ChunkPos>()) {
        neighbors_[side] = nullptr;
    }
}

void ChunkContainer::SetData(const ChunkRawData& data) {
    lighting_->ReplaceData(data.lighting_data_.GetData());
    block_storage_ = data.chunk_data_;
    position_ = data.pos_;
}

ChunkRawData ChunkContainer::GetRawData() {
    return ChunkRawData{block_storage_, *lighting_.get(), position_};
}

LightStorage ChunkContainer::GetLightData() {
    return LightStorage{*lighting_.get()};
}

void ChunkContainer::UpdateHeightMap() {
    for (int x = 0; x < kChunkDim; ++x) {
        for (int z = 0; z < kChunkDim; ++z) {
            UpdateHeightMap(x, z);
        }
    }
}

void ChunkContainer::UpdateHeightMap(int x, int z) {
    // Check chunk above first, if the heightmap above is > -1, it means that
    // there are block above -1 indicate theirs nothing in the column
    ChunkContainer* chunk_above = neighbors_[Directions<ChunkPos>::kUp];

    int new_height =
        -1;  // -1 is the default height if there is no blocks in the column
    int old_height = heightmap_->Get(x, z);

    if (chunk_above != nullptr && chunk_above->heightmap_->Get(x, z) != -1) {
        new_height = kChunkDim;
    } else {
        for (int i = 15; i >= 0; --i) {
            if (GetBlockUnsafe(BlockPos{x, i, z}) != g_blocks.AIR) {
                new_height = i;
                break;
            }
        }
    }

    if (new_height != old_height) {
        light_dirty_ = true;
        heightmap_->Edit(x, z, new_height);

        ChunkContainer* chunk_bottom = neighbors_[Directions<ChunkPos>::kDown];
        if (chunk_bottom != nullptr && (old_height == -1 || new_height == -1)) {
            chunk_bottom->UpdateHeightMap(x, z);
        }
    }
}

bool ChunkContainer::CheckLightDirty() {
    bool is_dirty = light_dirty_;
    light_dirty_ = false;
    return is_dirty;
}

void ChunkContainer::SetLightDirty() { light_dirty_ = true; }
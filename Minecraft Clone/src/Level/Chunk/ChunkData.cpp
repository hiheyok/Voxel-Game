// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Chunk/ChunkData.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/ChunkRawData.h"
#include "Level/Chunk/Heightmap/Heightmap.h"
#include "Level/TerrainGeneration/Structures/Structure.h"

ChunkContainer::ChunkContainer(GameContext& game_context)
    : game_context_{game_context},
      lighting_{std::make_unique<LightStorage>()},
      heightmap_{std::make_unique<HeightMap>()},
      outside_block_to_place_{6},
      neighbors_{6},
      block_storage_{} {
  lighting_->position_ = position_;
}

ChunkContainer::~ChunkContainer() = default;
ChunkContainer::ChunkContainer(ChunkContainer&&) = default;

ChunkContainer::ChunkContainer(GameContext& game_context,
                               const ChunkRawData& data)
    : ChunkContainer(game_context) {
  SetData(data);
}

BlockID ChunkContainer::GetBlock(BlockPos pos) const {
  if (static_cast<unsigned>(pos.x) >= kChunkDim ||
      static_cast<unsigned>(pos.y) >= kChunkDim ||
      static_cast<unsigned>(pos.z) >= kChunkDim) {
    int dx = ((pos.x >> 31) & 1) + 0;
    int dy = ((pos.y >> 31) & 1) + 2;
    int dz = ((pos.z >> 31) & 1) + 4;

    if (neighbors_[dx].has_value() && (pos.x >> kChunkDimLog2))
      return neighbors_[dx].value()->GetBlock(
          pos.IncrementSide(dx, -kChunkDim));
    if (neighbors_[dy].has_value() && (pos.y >> kChunkDimLog2))
      return neighbors_[dy].value()->GetBlock(
          pos.IncrementSide(dy, -kChunkDim));
    if (neighbors_[dz].has_value() && (pos.z >> kChunkDimLog2))
      return neighbors_[dz].value()->GetBlock(
          pos.IncrementSide(dz, -kChunkDim));

    return game_context_.blocks_->AIR;
  } else {
    return GetBlockUnsafe(pos);
  }
}

BlockID ChunkContainer::GetBlockUnsafe(BlockPos pos) const noexcept {
  return block_storage_.GetBlockUnsafe(pos);
}

void ChunkContainer::SetBlock(BlockID block, BlockPos pos) {
  if (static_cast<unsigned>(pos.x) >= kChunkDim ||
      static_cast<unsigned>(pos.y) >= kChunkDim ||
      static_cast<unsigned>(pos.z) >= kChunkDim) {
    int dx = ((pos.x >> 31) & 1) + 0;
    int dy = ((pos.y >> 31) & 1) + 2;
    int dz = ((pos.z >> 31) & 1) + 4;

    if (pos.x >> kChunkDimLog2) {
      outside_block_to_place_[dx].emplace_back(
          block, pos.IncrementSide(dx, -kChunkDim));
      return;
    }
    if (pos.y >> kChunkDimLog2) {
      outside_block_to_place_[dy].emplace_back(
          block, pos.IncrementSide(dy, -kChunkDim));
      return;
    }
    if (pos.z >> kChunkDimLog2) {
      outside_block_to_place_[dz].emplace_back(
          block, pos.IncrementSide(dz, -kChunkDim));
      return;
    }
  } else {
    SetBlockUnsafe(block, pos);
  }
}

void ChunkContainer::SetBlockUnsafe(BlockID block, BlockPos pos) {
  block_storage_.SetBlockUnsafe(block, pos);
  is_empty_ = false;
}

void ChunkContainer::SetPosition(ChunkPos pos) noexcept {
  position_ = pos;
  lighting_->position_ = pos;
}
std::optional<ChunkContainer*> ChunkContainer::GetNeighbor(
    int side) const noexcept {
  return neighbors_[side];
}

void ChunkContainer::SetNeighbor(ChunkContainer* neighbor, int side) {
  neighbors_[side] = neighbor;
}

void ChunkContainer::ClearNeighbors() {
  for (const auto& side : Directions<ChunkPos>()) {
    neighbors_[side].reset();
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
  std::optional<ChunkContainer*> chunk_above =
      neighbors_[Directions<ChunkPos>::kUp];

  int new_height =
      -1;  // -1 is the default height if there is no blocks in the column
  int old_height = heightmap_->Get(x, z);

  if (chunk_above.has_value() &&
      chunk_above.value()->heightmap_->Get(x, z) != -1) {
    new_height = kChunkDim;
  } else {
    for (int i = 15; i >= 0; --i) {
      if (GetBlockUnsafe(BlockPos{x, i, z}) != game_context_.blocks_->AIR) {
        new_height = i;
        break;
      }
    }
  }

  if (new_height != old_height) {
    light_dirty_ = true;
    heightmap_->Edit(x, z, new_height);

    std::optional<ChunkContainer*> chunk_bottom =
        neighbors_[Directions<ChunkPos>::kDown];
    if (chunk_bottom.has_value() && (old_height == -1 || new_height == -1)) {
      chunk_bottom.value()->UpdateHeightMap(x, z);
    }
  }
}

bool ChunkContainer::CheckLightDirty() {
  bool is_dirty = light_dirty_;
  light_dirty_ = false;
  return is_dirty;
}

void ChunkContainer::SetLightDirty() { light_dirty_ = true; }

const Palette& ChunkContainer::GetPalette() const {
  return block_storage_;
}

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Chunk/ChunkData.h"

#include <array>
#include <atomic>
#include <optional>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/ChunkRawData.h"
#include "Level/Chunk/Heightmap/Heightmap.h"
#include "Level/TerrainGeneration/Structures/Structure.h"
#include "Utils/Iterators.h"

using std::make_unique;
using std::move;
using std::vector;

ChunkContainer::ChunkContainer(GameContext& context)
    : context_{context},
      sky_light_{make_unique<LightStorage>()},
      block_light_{make_unique<LightStorage>()},
      heightmap_{make_unique<HeightMap>()},
      outside_block_to_place_{},
      neighbors_{nullptr},
      block_storage_{} {
  sky_light_->position_ = position_;
  block_light_->position_ = position_;
}

ChunkContainer::~ChunkContainer() = default;
ChunkContainer::ChunkContainer(ChunkContainer&& other)
    : context_{other.context_},
      position_{other.position_},
      sky_light_{move(other.sky_light_)},
      block_light_{move(other.block_light_)},
      heightmap_{move(other.heightmap_)},
      outside_block_to_place_{move(other.outside_block_to_place_)},
      is_empty_{other.is_empty_},
      neighbors_{move(other.neighbors_)},
      block_storage_{move(other.block_storage_)} {
  light_dirty_.store(other.light_dirty_.load(std::memory_order_relaxed),
                     std::memory_order_relaxed);
  is_light_up_.store(other.is_light_up_.load(std::memory_order_relaxed),
                     std::memory_order_relaxed);
}

ChunkContainer::ChunkContainer(GameContext& context, const ChunkRawData& data)
    : ChunkContainer(context) {
  SetData(data);
}

BlockID ChunkContainer::GetBlock(BlockPos pos) const noexcept {
  if (static_cast<unsigned>(pos.x) >= kChunkDim ||
      static_cast<unsigned>(pos.y) >= kChunkDim ||
      static_cast<unsigned>(pos.z) >= kChunkDim) {
    int dx = static_cast<int>(pos.x < 0) + 0;
    int dy = static_cast<int>(pos.y < 0) + 2;
    int dz = static_cast<int>(pos.z < 0) + 4;

    if (neighbors_[dx] && (pos.x >> kChunkDimLog2))
      return neighbors_[dx]->GetBlock(pos.IncrementSide(dx, -kChunkDim));
    if (neighbors_[dy] && (pos.y >> kChunkDimLog2))
      return neighbors_[dy]->GetBlock(pos.IncrementSide(dy, -kChunkDim));
    if (neighbors_[dz] && (pos.z >> kChunkDimLog2))
      return neighbors_[dz]->GetBlock(pos.IncrementSide(dz, -kChunkDim));

    return context_.blocks_->AIR;
  } else {
    return GetBlockUnsafe(pos);
  }
}

BlockID ChunkContainer::GetBlockUnsafe(BlockPos pos) const noexcept {
  return block_storage_.GetBlock(pos);
}

const ChunkContainer* ChunkContainer::GetChunkAtBlockPos(
    BlockPos pos) const noexcept {
  if (static_cast<unsigned>(pos.x) >= kChunkDim ||
      static_cast<unsigned>(pos.y) >= kChunkDim ||
      static_cast<unsigned>(pos.z) >= kChunkDim) {
    int dx = static_cast<int>(pos.x < 0) + 0;
    int dy = static_cast<int>(pos.y < 0) + 2;
    int dz = static_cast<int>(pos.z < 0) + 4;

    if (neighbors_[dx] && (pos.x >> kChunkDimLog2))
      return neighbors_[dx]->GetChunkAtBlockPos(
          pos.IncrementSide(dx, -kChunkDim));
    if (neighbors_[dy] && (pos.y >> kChunkDimLog2))
      return neighbors_[dy]->GetChunkAtBlockPos(
          pos.IncrementSide(dy, -kChunkDim));
    if (neighbors_[dz] && (pos.z >> kChunkDimLog2))
      return neighbors_[dz]->GetChunkAtBlockPos(
          pos.IncrementSide(dz, -kChunkDim));

    return nullptr;
  } else {
    return this;
  }
}

void ChunkContainer::SetLightLvl(BlockPos pos, bool is_sky, int lvl) noexcept {
  if (is_sky) {
    if (sky_light_->GetLighting(pos) != lvl) {
      SetLightDirty();
    }
    return sky_light_->EditLight(pos, lvl);
  } else {
    if (block_light_->GetLighting(pos) != lvl) {
      SetLightDirty();
    }
    return block_light_->EditLight(pos, lvl);
  }
}

int ChunkContainer::GetLightLvl(BlockPos pos, bool is_sky) const noexcept {
  if (is_sky) {
    return sky_light_->GetLighting(pos);
  } else {
    return block_light_->GetLighting(pos);
  }
}

void ChunkContainer::SetBlock(BlockID block, BlockPos pos) {
  if (static_cast<unsigned>(pos.x) >= kChunkDim ||
      static_cast<unsigned>(pos.y) >= kChunkDim ||
      static_cast<unsigned>(pos.z) >= kChunkDim) {
    int dx = static_cast<int>(pos.x < 0) + 0;
    int dy = static_cast<int>(pos.y < 0) + 2;
    int dz = static_cast<int>(pos.z < 0) + 4;

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
  block_storage_.SetBlock(block, pos);
  is_empty_ = false;
}

void ChunkContainer::SetPosition(ChunkPos pos) noexcept {
  position_ = pos;
  sky_light_->position_ = pos;
  block_light_->position_ = pos;
}
std::optional<ChunkContainer*> ChunkContainer::GetNeighbor(
    int side) const noexcept {
  if (neighbors_[side] == nullptr) {
    return std::nullopt;
  }
  return neighbors_[side];
}

void ChunkContainer::SetNeighbor(ChunkContainer* neighbor, int side) noexcept {
  neighbors_[side] = neighbor;
}

void ChunkContainer::ClearNeighbors() noexcept { neighbors_.fill(nullptr); }

void ChunkContainer::SetData(const ChunkRawData& data) {
  *sky_light_.get() = data.sky_light_;
  *block_light_.get() = data.block_light_;
  block_storage_ = data.chunk_data_;
  position_ = data.pos_;
}

ChunkRawData ChunkContainer::GetRawData() {
  return ChunkRawData{block_storage_, *sky_light_.get(), *block_light_.get(),
                      position_};
}

void ChunkContainer::UpdateHeightMap() {
  for (auto [x, z] : Product<2>(kChunkDim)) {
    UpdateHeightMap(x, z);
  }
}

void ChunkContainer::UpdateHeightMap(int x, int z) {
  const vector<BlockProperties>& properties =
      context_.blocks_->GetBlockPropertyList();

  // Check chunk above first, if the heightmap above is > -1, it means that
  // there are block above -1 indicate theirs nothing in the column
  auto chunk_above = neighbors_[Directions<ChunkPos>::kUp];
  // -1 is the default height if there is no blocks in the column
  int new_height = -1;
  int old_height = heightmap_->Get(x, z);

  if (chunk_above && chunk_above->heightmap_->Get(x, z) != -1) {
    new_height = kChunkDim;
  } else {
    for (int i = kChunkDim - 1; i >= 0; --i) {
      if (properties[GetBlockUnsafe({x, i, z})].opacity_ > 0) {
        new_height = i;
        break;
      }
    }
  }

  if (new_height != old_height) {
    SetLightDirty();
    heightmap_->Edit(x, z, new_height);

    auto chunk_bottom = neighbors_[Directions<ChunkPos>::kDown];
    if (chunk_bottom && (old_height == -1 || new_height == -1)) {
      chunk_bottom->UpdateHeightMap(x, z);
    }
  }
}

bool ChunkContainer::CheckLightDirty() noexcept {
  return light_dirty_.exchange(false, std::memory_order_acq_rel);
}

void ChunkContainer::SetLightDirty() noexcept {
  light_dirty_.store(true, std::memory_order_release);
}

bool ChunkContainer::IsLightUp() const noexcept {
  return is_light_up_.load(std::memory_order_relaxed);
}
void ChunkContainer::SetLightUp(bool is_light_up) noexcept {
  is_light_up_.store(is_light_up, std::memory_order_release);
}

const Palette& ChunkContainer::GetPalette() const noexcept {
  return block_storage_;
}

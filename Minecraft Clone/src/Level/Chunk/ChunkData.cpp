// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Chunk/ChunkData.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/ChunkRawData.h"
#include "Level/Chunk/Heightmap/Heightmap.h"
#include "Level/TerrainGeneration/Structures/Structure.h"

ChunkContainer::ChunkContainer(GameContext& context)
    : context_{context},
      sky_light_{std::make_unique<LightStorage>()},
      block_light_{std::make_unique<LightStorage>()},
      heightmap_{std::make_unique<HeightMap>()},
      outside_block_to_place_{6},
      neighbors_{6},
      block_storage_{} {
  sky_light_->position_ = position_;
  block_light_->position_ = position_;
}

ChunkContainer::~ChunkContainer() = default;
ChunkContainer::ChunkContainer(ChunkContainer&& other)
    : context_{other.context_},
      sky_light_{std::move(other.sky_light_)},
      block_light_{std::move(other.block_light_)},
      heightmap_{std::move(other.heightmap_)},
      outside_block_to_place_{std::move(other.outside_block_to_place_)},
      neighbors_{std::move(other.neighbors_)},
      block_storage_{std::move(other.block_storage_)} {
  light_dirty_.store(other.light_dirty_.load(std::memory_order_release),
                     std::memory_order_acquire);
}

ChunkContainer::ChunkContainer(GameContext& context, const ChunkRawData& data)
    : ChunkContainer(context) {
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

    return context_.blocks_->AIR;
  } else {
    return GetBlockUnsafe(pos);
  }
}

BlockID ChunkContainer::GetBlockUnsafe(BlockPos pos) const noexcept {
  return block_storage_.GetBlockUnsafe(pos);
}

void ChunkContainer::SetLightLvl(BlockPos pos, bool is_sky, int lvl) {
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

int ChunkContainer::GetLightLvl(BlockPos pos, bool is_sky) const {
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
  sky_light_->position_ = pos;
  block_light_->position_ = pos;
}
std::optional<ChunkContainer*> ChunkContainer::GetNeighbor(
    int side) const noexcept {
  return neighbors_[side];
}

void ChunkContainer::SetNeighbor(ChunkContainer* neighbor, int side) {
  neighbors_[side] = neighbor;
}

void ChunkContainer::ClearNeighbors() {
  for (auto side : Directions<ChunkPos>()) {
    neighbors_[side].reset();
  }
}

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
  const std::vector<BlockProperties>& properties =
      context_.blocks_->GetBlockPropertyList();

  // Check chunk above first, if the heightmap above is > -1, it means that
  // there are block above -1 indicate theirs nothing in the column
  auto chunk_above = neighbors_[Directions<ChunkPos>::kUp];

  int new_height =
      -1;  // -1 is the default height if there is no blocks in the column
  int old_height = heightmap_->Get(x, z);

  if (chunk_above.has_value() &&
      chunk_above.value()->heightmap_->Get(x, z) != -1) {
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
    light_dirty_ = true;
    heightmap_->Edit(x, z, new_height);

    auto chunk_bottom = neighbors_[Directions<ChunkPos>::kDown];
    if (chunk_bottom.has_value() && (old_height == -1 || new_height == -1)) {
      chunk_bottom.value()->UpdateHeightMap(x, z);
    }
  }
}

bool ChunkContainer::CheckLightDirty() {
  bool is_dirty = light_dirty_.load(std::memory_order_acquire);
  light_dirty_.store(false, std::memory_order_release);
  return is_dirty;
}

void ChunkContainer::SetLightDirty() {
  light_dirty_.store(true, std::memory_order_release);
}

bool ChunkContainer::IsLightUp() const noexcept {
  return is_light_up_.load(std::memory_order_relaxed);
}
void ChunkContainer::SetLightUp(bool is_light_up) {
  is_light_up_.store(is_light_up, std::memory_order_acquire);
}

const Palette& ChunkContainer::GetPalette() const { return block_storage_; }

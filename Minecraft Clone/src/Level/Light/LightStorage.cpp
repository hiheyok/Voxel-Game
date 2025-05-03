// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Light/LightStorage.h"

LightStorage::LightStorage() { ResetLighting(); }

LightStorage::LightStorage(LightStorage&&) noexcept = default;
LightStorage::LightStorage(const LightStorage&) noexcept = default;

LightStorage::~LightStorage() = default;

LightStorage& LightStorage::operator=(const LightStorage&) noexcept = default;
LightStorage& LightStorage::operator=(LightStorage&&) noexcept = default;

bool LightStorage::operator==(const LightStorage& other) const noexcept {
  int len = kChunkSize3D * 4 / 64;
  for (int i = 0; i < len; ++i) {
    if (other.data_[i] != data_[i]) return false;
  }
  return true;
}

const uint64_t* LightStorage::GetData() const { return data_; }

void LightStorage::ReplaceData(const uint64_t* src) {
  memcpy(data_, src, sizeof(uint64_t) * 256);
}

void LightStorage::EditLight(BlockPos pos, uint8_t lightingInfo) noexcept {
  static constexpr uint64_t mask = kChunkDim - 1;

  data_[(pos.x << 4) | (pos.z)] &= (~(mask << (pos.y << 2)));

  data_[(pos.x << 4) | (pos.z)] |=
      (static_cast<uint64_t>(lightingInfo) << (pos.y << 2));
}

uint8_t LightStorage::GetLighting(BlockPos pos) const noexcept {
  static constexpr uint64_t mask = kChunkDim - 1;
  return (data_[(pos.x << 4) | (pos.z)] >> (pos.y << 2)) & mask;
}

void LightStorage::ResetLighting() {
  memset(reinterpret_cast<uint8_t*>(data_), 0,
         kChunkSize2D * sizeof(uint64_t));  // 8 = sizeof uint64_t
}

void LightStorage::ResetLightingCustom(uint8_t lvl) {
  memset(reinterpret_cast<uint8_t*>(data_), lvl | (lvl << 4),
         kChunkSize2D * sizeof(uint64_t));  // 8 = sizeof uint64_t
}

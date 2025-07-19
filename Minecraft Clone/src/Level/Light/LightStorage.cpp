// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Light/LightStorage.h"

LightStorage::LightStorage() { ResetLighting(); }

LightStorage::LightStorage(LightStorage&&) noexcept = default;
LightStorage::LightStorage(const LightStorage&) noexcept = default;

LightStorage::~LightStorage() = default;

LightStorage& LightStorage::operator=(const LightStorage&) noexcept = default;
LightStorage& LightStorage::operator=(LightStorage&&) noexcept = default;

bool LightStorage::operator==(const LightStorage& other) const noexcept {
  return data_ == other.data_;
}

void LightStorage::EditLight(BlockPos pos, uint8_t lighting_info) noexcept {
  static constexpr uint64_t mask = kChunkDim - 1;
  size_t idx = pos.GetIndex();
  data_.Set(idx, lighting_info);
}

uint8_t LightStorage::GetLighting(BlockPos pos) const noexcept {
  return data_.Get(pos.GetIndex());
}

void LightStorage::ResetLighting() { data_.Fill(0); }

void LightStorage::ResetLightingCustom(uint8_t lvl) { data_.Fill(lvl); }

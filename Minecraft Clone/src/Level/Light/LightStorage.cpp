// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Light/LightStorage.h"

#include <cstddef>
#include <cstdint>

#include "Core/Position/PositionTypes.h"

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
  size_t idx = pos.GetIndex();
  data_.Set(idx, lighting_info);
}

void LightStorage::ResetLighting() noexcept { data_.Fill(0); }

void LightStorage::ResetLightingCustom(uint8_t lvl) noexcept {
  data_.Fill(lvl);
}

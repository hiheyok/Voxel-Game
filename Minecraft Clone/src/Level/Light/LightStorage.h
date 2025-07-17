// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <cctype>

#include "Core/Typenames.h"
#include "Core/DataStructure/AtomicNibbleArray.h"
#include "Core/DataStructure/NibbleArray.h"

class LightStorage {  // Contains all lighting infomation for solid blocks
 public:
  LightStorage();
  LightStorage(const LightStorage&) noexcept;
  LightStorage(LightStorage&&) noexcept;
  ~LightStorage();

  LightStorage& operator=(const LightStorage&) noexcept;
  LightStorage& operator=(LightStorage&&) noexcept;

  bool operator==(const LightStorage&) const noexcept;

  void EditLight(BlockPos pos, uint8_t LightingInfo) noexcept;

  uint8_t GetLighting(BlockPos pos) const noexcept;

  void ResetLighting();

  void ResetLightingCustom(uint8_t lvl);

  static const uint8_t kMaxLightLevel = 15;
  ChunkPos position_;

 private:
  alignas(64) NibbleArray<kChunkSize3D> data_;
};

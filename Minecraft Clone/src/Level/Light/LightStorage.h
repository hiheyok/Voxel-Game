// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <cctype>

#include "Core/DataStructure/AtomicNibbleArray.h"
#include "Core/DataStructure/NibbleArray.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"

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
  
  FORCEINLINE uint8_t GetLighting(BlockPos pos) const noexcept {
    return data_.Get(pos.GetIndex());
  }
  
  // Fast path: use pre-computed index directly
  FORCEINLINE uint8_t GetLightingByIndex(size_t idx) const noexcept {
    return data_.Get(idx);
  }

  void ResetLighting() noexcept;

  void ResetLightingCustom(uint8_t lvl) noexcept;

  static const uint8_t kMaxLightLevel = 15;
  ChunkPos position_;

 private:
  alignas(64) NibbleArray<kChunkSize3D> data_;
};

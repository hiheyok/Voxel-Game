#pragma once
#include <cctype>

#include "Core/Typenames.h"

class LightStorage {  // Contains all lighting infomation for solid blocks
 public:
  LightStorage();
  LightStorage(const LightStorage&);
  LightStorage(LightStorage&&);
  ~LightStorage();

  LightStorage& operator=(const LightStorage&) = default;
  LightStorage& operator=(LightStorage&&) = default;

  bool operator==(const LightStorage&) const;

  const uint64_t* GetData() const;

  void ReplaceData(const uint64_t* src);

  void EditLight(BlockPos pos, uint8_t LightingInfo);

  uint8_t GetLighting(BlockPos pos) const;

  void ResetLighting();

  void ResetLightingCustom(uint8_t lvl);

  static const uint8_t kMaxLightLevel = 15;
  ChunkPos position_;

 private:
  uint64_t data_[256]{};  // TODO use std::array<uint64_t, kChunkSize2D>
};

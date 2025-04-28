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

    void EditLight(const BlockPos& pos, unsigned char LightingInfo);

    uint8_t GetLighting(const BlockPos& pos) const;

    void ResetLighting();

    void ResetLightingCustom(uint8_t lvl);

    static const unsigned char kMaxLightLevel = 15;
    ChunkPos position_;

   private:
    uint64_t data_[256]{};  // x z y
};
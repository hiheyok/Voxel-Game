#pragma once
#include <cctype>

#include "Core/Typenames.h"

class ChunkLightingContainer { //Contains all lighting infomation for solid blocks
public:
    static const unsigned char MaxLightLevel = 15;
    ChunkPos position_;

    ChunkLightingContainer();
    ChunkLightingContainer(const ChunkLightingContainer&);
    ChunkLightingContainer(ChunkLightingContainer&&);
    ~ChunkLightingContainer();

    ChunkLightingContainer& operator=(const ChunkLightingContainer&) = default;
    ChunkLightingContainer& operator=(ChunkLightingContainer&&) = default;

    const uint64_t* getData() const;

    void ReplaceData(const uint64_t* src);

    void EditLight(int x, int y, int z, unsigned char LightingInfo);

    uint8_t GetLighting(int x, int y, int z) const;

    void ResetLighting();

    void ResetLightingCustom(uint8_t lvl);
private:
    uint64_t data_[256]{}; //x z y
};
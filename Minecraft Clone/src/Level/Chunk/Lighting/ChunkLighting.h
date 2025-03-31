#pragma once
#include <cctype>

#include "../../Typenames.h"

class ChunkLightingContainer { //Contains all lighting infomation for solid blocks
private:
    uint64_t data_[256]{}; //x z y
public:

    static const unsigned char MaxLightLevel = 15;
    ChunkPos position_;

    ChunkLightingContainer();
    ~ChunkLightingContainer();

    uint64_t* getData();

    void ReplaceData(uint64_t* src);

    void EditLight(int x, int y, int z, unsigned char LightingInfo);

    uint8_t GetLighting(int x, int y, int z) const;

    void ResetLighting();

    void ResetLightingCustom(uint8_t lvl);

};
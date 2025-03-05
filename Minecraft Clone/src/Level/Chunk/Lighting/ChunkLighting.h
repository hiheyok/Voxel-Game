#pragma once
#include <string.h>
#include <iostream>

class ChunkLightingContainer { //Contains all lighting infomation for solid blocks
private:
    uint64_t data_[256]{}; //x z y

public:

    static const unsigned char MaxLightLevel = 15;
    ChunkPos position_;

    ChunkLightingContainer() {
        ResetLighting();
    }

    inline uint64_t* getData() {
        return data_;
    }

    inline void ReplaceData(uint64_t* src) {
        memcpy(data_, src, sizeof(uint64_t) * 256);
    }

    inline void EditLight(int x, int y, int z, unsigned char LightingInfo) {
        data_[(x << 4) | (z)] &= (~(0b1111ULL << (y << 2)));

        data_[(x << 4) | (z)] |= ((uint64_t)LightingInfo << (y << 2));
    }

    inline uint8_t GetLighting(int x, int y, int z) const {
        return (data_[(x << 4) | (z)] >> (y << 2)) & 0b1111;
    }

    inline void ResetLighting() {
        memset((uint8_t*)data_, MaxLightLevel | (MaxLightLevel << 4), 256 * 8); //8 = sizeof uint64_t
    }

    inline void ResetLightingCustom(uint8_t lvl) {
        memset((uint8_t*)data_, lvl | (lvl << 4), 256 * 8); //8 = sizeof uint64_t
    }

};
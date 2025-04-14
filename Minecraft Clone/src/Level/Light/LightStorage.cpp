#include "Level/Light/LightStorage.h"

LightStorage::LightStorage() {
    ResetLighting();
}

LightStorage::LightStorage(LightStorage&&) = default;
LightStorage::LightStorage(const LightStorage&) = default;

LightStorage::~LightStorage() = default;

const uint64_t* LightStorage::GetData() const {
    return data_;
}

void LightStorage::ReplaceData(const uint64_t* src) {
    memcpy(data_, src, sizeof(uint64_t) * 256);
}

void LightStorage::EditLight(int x, int y, int z, unsigned char lightingInfo) {
    data_[(x << 4) | (z)] &= (~(0b1111ULL << (y << 2)));

    data_[(x << 4) | (z)] |= ((uint64_t)lightingInfo << (y << 2));
}

uint8_t LightStorage::GetLighting(int x, int y, int z) const {
    return (data_[(x << 4) | (z)] >> (y << 2)) & 0b1111;
}

void LightStorage::ResetLighting() {
    memset((uint8_t*)data_, 0, 256 * 8); //8 = sizeof uint64_t
}

void LightStorage::ResetLightingCustom(uint8_t lvl) {
    memset((uint8_t*)data_, lvl | (lvl << 4), 256 * 8); //8 = sizeof uint64_t
}
#include "Level/Chunk/Lighting/ChunkLighting.h"

ChunkLightingContainer::ChunkLightingContainer() {
    ResetLighting();
}

ChunkLightingContainer::ChunkLightingContainer(ChunkLightingContainer&&) = default;
ChunkLightingContainer::ChunkLightingContainer(const ChunkLightingContainer&) = default;


ChunkLightingContainer::~ChunkLightingContainer() = default;

const uint64_t* ChunkLightingContainer::getData() const {
    return data_;
}

void ChunkLightingContainer::ReplaceData(const uint64_t* src) {
    memcpy(data_, src, sizeof(uint64_t) * 256);
}

void ChunkLightingContainer::EditLight(int x, int y, int z, unsigned char lightingInfo) {
    data_[(x << 4) | (z)] &= (~(0b1111ULL << (y << 2)));

    data_[(x << 4) | (z)] |= ((uint64_t)lightingInfo << (y << 2));
}

uint8_t ChunkLightingContainer::GetLighting(int x, int y, int z) const {
    return (data_[(x << 4) | (z)] >> (y << 2)) & 0b1111;
}

void ChunkLightingContainer::ResetLighting() {
    memset((uint8_t*)data_, MaxLightLevel | (MaxLightLevel << 4), 256 * 8); //8 = sizeof uint64_t
}

void ChunkLightingContainer::ResetLightingCustom(uint8_t lvl) {
    memset((uint8_t*)data_, lvl | (lvl << 4), 256 * 8); //8 = sizeof uint64_t
}
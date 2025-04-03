#include "Core//Typenames.h"
#include "Level/Chunk/Heightmap/Heightmap.h"

Heightmap::Heightmap() {
    data_.resize(256);
}
Heightmap::~Heightmap() = default;

void Heightmap::Edit(int x, int z, int height) {
    data_[x * kChunkDim + z] =  height;
}

int16_t Heightmap::Get(int x, int z) const {
    return data_[x * kChunkDim + z];
}

void Heightmap::Clear() {
    data_.clear();
}
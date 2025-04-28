#include "Level/Chunk/Heightmap/Heightmap.h"

#include "Core//Typenames.h"

Heightmap::Heightmap() { data_.fill(-1); }
Heightmap::~Heightmap() = default;

void Heightmap::Edit(int x, int z, int height) {
    data_[x * kChunkDim + z] = height;
}

int Heightmap::Get(int x, int z) const { return data_[x * kChunkDim + z]; }

void Heightmap::Clear() { data_.fill(-1); }
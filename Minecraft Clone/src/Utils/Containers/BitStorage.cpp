#include "Utils/Containers/BitStorage.h"

void BitStorage3D::Initialize(int x, int y, int z) {  // Set Sizes
    int nBits = x * y * z;

    int mapsize = nBits >> 6;

    if (nBits & 0b111111) {
        mapsize++;
    }

    map_.resize(mapsize);
}

void BitStorage3D::ClearBit(int x, int y, int z) {
    int index = x * (sy_ * sz_) + y * (sz_) + z;

    size_t NumIndex = index >> 6;
    size_t Index = index & 0b111111;

    map_[NumIndex] &= (~(0b1LL << Index));
}

void BitStorage3D::Clear() {
    for (int i = 0; i < map_.size(); i++) {
        map_[i] = 0;
    }
}

void BitStorage3D::SetBit(int x, int y, int z) {
    int index = x * (sy_ * sz_) + y * (sz_) + z;

    size_t NumIndex = index >> 6;
    size_t Index = index & 0b111111;

    map_[NumIndex] |= (0b1LL << Index);
}

void BitStorage3D::SetBit(int* pos) { SetBit(pos[0], pos[1], pos[2]); }

bool BitStorage3D::GetBit(int x, int y, int z) {
    int index = x * (sy_ * sz_) + y * (sz_) + z;

    size_t numIndex = index >> 6;
    size_t bitIndex = index & 0b111111;
    return (map_[numIndex] >> bitIndex) & 0b1;
}

bool BitStorage3D::GetBit(int* pos) { return GetBit(pos[0], pos[1], pos[2]); }
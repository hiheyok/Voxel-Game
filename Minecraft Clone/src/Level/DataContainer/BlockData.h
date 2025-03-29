#pragma once
#include "../Chunk/Block/Blocks.h"
#include <cctype>

class BlockContainer {
public:
    
    const BlockID& GetBlock(unsigned int x, unsigned int y, unsigned int z) const {
        return data_[GetDataIndex(x,y,z)];
    }

    void SetBlock(BlockID ID, unsigned int x, unsigned int y, unsigned int z) {
        data_[GetDataIndex(x,y,z)] = ID;
    }

    const BlockID& GetBlockUnsafe(unsigned int index) const {
        return data_[index];
    }

    void SetBlockUnsafe(BlockID ID, unsigned int index) {
        data_[index] = ID;
    }

    const BlockID* GetContainerData() const {
        return data_;
    }

private:

    constexpr static uint16_t GetDataIndex(unsigned int x, unsigned int y, unsigned int z) {
        return (x << 8) | (z << 4) | y;
    }
    
    BlockID data_[4096]{g_blocks.AIR};
};

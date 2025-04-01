#pragma once
#include <cctype>
#include "Level/Chunk/Block/Block.h"

struct SetBlockRelative {
    int8_t x_;
    int8_t y_;
    int8_t z_;

    BlockID block_;

    SetBlockRelative(BlockID block, int8_t x, int8_t y, int8_t z);

    void SetPos(int8_t x, int8_t y, int8_t z);
};

struct Structure {
    std::vector<SetBlockRelative> build_;
};
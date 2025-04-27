#pragma once
#include <cctype>
#include "Level/Block/Block.h"

struct SetBlockRelative {
    BlockPos pos_;

    BlockID block_;

    SetBlockRelative(BlockID block, const BlockPos& pos);
    ~SetBlockRelative();
};

struct Structure {
    std::vector<SetBlockRelative> build_;
};
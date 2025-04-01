#pragma once
#include "../../Typenames.h"

namespace BlockUpdatePacket {
    struct BlockUpdate {
        BlockID block_;
        BlockPos pos_;
        BlockUpdate() = default;
    };
}
#pragma once
#include "Core/Typenames.h"

namespace BlockUpdatePacket {
struct BlockUpdate {
  BlockID block_;
  BlockPos pos_;
  BlockUpdate() = default;
};
}  // namespace BlockUpdatePacket

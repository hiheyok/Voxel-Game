#pragma once
#include <cctype>
#include <vector>

#include "Level/Block/Block.h"

struct SetBlockRelative {
  BlockPos pos_;
  BlockID block_;

  SetBlockRelative(BlockID block, BlockPos pos);
  ~SetBlockRelative();
};

struct Structure {
  std::vector<SetBlockRelative> build_;
};

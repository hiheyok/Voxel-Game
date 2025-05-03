// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "Level/Block/Blocks.h"
#include "Level/Item/Item.h"

class BlockItem : public Item {
 public:
  BlockItem(BlockID block_) {
    properties_.block_ = block_;
    properties_.is_block_item_ = true;
  }

 private:
};

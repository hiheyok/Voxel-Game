// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <string>

#include "Core/Typenames.h"

struct ItemProperties {
  int max_stack_ = 64;
  bool is_block_item_ = false;
  BlockID block_ = 0;
  ItemID id_ = 0;
  std::string name_ = "";

  bool operator==(const ItemProperties& other) const noexcept;
};

class Item {
 public:
  Item();
  ~Item();

  bool operator==(const Item& other) const noexcept;

  BlockID GetBlock() const;
  bool CheckIsBlockItem() const;

  ItemProperties properties_;

 private:
};

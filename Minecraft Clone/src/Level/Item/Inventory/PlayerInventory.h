// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "Level/Block/Blocks.h"
#include "Level/Item/Inventory/Inventory.h"
#include "Level/Item/Items.h"

class PlayerInventory : public Inventory {
 public:
  PlayerInventory() : Inventory{g_items.GetTotalItemCount()} {}
};

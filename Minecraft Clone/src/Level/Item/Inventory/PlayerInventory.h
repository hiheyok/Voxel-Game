#pragma once

#include "Level/Item/Inventory/Inventory.h"
#include "Level/Block/Blocks.h"
#include "Level/Item/Items.h"

class PlayerInventory : public Inventory {
public:
    PlayerInventory() : Inventory{ g_items.GetTotalItemCount() } {}
};
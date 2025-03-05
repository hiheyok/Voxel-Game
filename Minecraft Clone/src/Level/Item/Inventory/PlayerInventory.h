#pragma once

#include "Inventory.h"
#include "../../Chunk/Block/Blocks.h"
#include "../../Item/Items.h"
class PlayerInventory : public Inventory {
public:
    PlayerInventory() : Inventory{ g_items.GetTotalItemCount() } {}
};
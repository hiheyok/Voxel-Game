#pragma once
#include "Level/Item/Inventory/Inventory.h"
#include "Level/Block/Blocks.h"
#include "Level/Item/Items.h"
class EntityInventory : public Inventory {
public:
    EntityInventory(int slotCount = 9) {
        Initialize(slotCount);
    }

    int right_hand_slot_ = 0;
    int left_hand_slot_ = 0;
private:
};
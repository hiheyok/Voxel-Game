#pragma once
#include <vector>

#include "Core/Typenames.h"
#include "Level/Chunk/Block/Blocks.h"
#include "Level/Item/ItemStack.h"

class Inventory {
public:

    Inventory(int inventorySize = 0x7FFFFFFF) {
        if (inventorySize == 0x7FFFFFFF)
            return;
        Initialize(inventorySize);
    }

    ~Inventory() {
        storage_.clear();
    }

    ItemStack GetItem(int slot);

    void SetSlot(int slot, ItemStack item);
    
    int GetSlotCount();

    void ChangeSlotSize(int slotCount);

protected:
    void Initialize(int slotCount);

private:
    
    int num_slots_ = 64;
    std::vector<ItemStack> storage_;

};
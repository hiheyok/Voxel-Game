#pragma once

#include "Level/Item/Item.h"

class ItemStack {  // Contains additional properties of item such as item count
                   // enchantments, etc
   public:
    ItemStack() {}

    ItemStack(Item item) {
        item_ = item;
        initialized_ = true;
    }

    bool IsInitialized();

    Item item_;
    int item_count_ = 1;

   private:
    bool initialized_ = false;
};
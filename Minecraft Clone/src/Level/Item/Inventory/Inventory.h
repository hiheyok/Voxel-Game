#pragma once
#include "../../Typenames.h"
#include "../../Chunk/Block/Blocks.h"
#include "../ItemStack.h"
#include <unordered_map>
#include <vector>

class Inventory {
public:

	Inventory(int inventorySize = 0xFFFFFFFF) {
		if (inventorySize == 0xFFFFFFFF)
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
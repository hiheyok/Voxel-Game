#pragma once
#include "../../Typenames.h"
#include "../../Chunk/Block/Blocks.h"
#include "../ItemStack.h"
#include <unordered_map>
#include <vector>

class Inventory {
public:

	Inventory(int InventorySize = 0xFFFFFFFF) {
		if (InventorySize == 0xFFFFFFFF)
			return;
		Initialize(InventorySize);
	}

	~Inventory() {
		Storage.clear();
	}

	ItemStack GetItem(int slot);

	void SetSlot(int slot, ItemStack item);
	
	int GetSlotCount();

	void ChangeSlotSize(int slotCount);

protected:
	void Initialize(int slotCount);

private:
	
	int NumOfSlots = 64;
	std::vector<ItemStack> Storage;

};
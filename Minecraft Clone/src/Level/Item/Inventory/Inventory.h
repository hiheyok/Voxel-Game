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

	ItemStack GetItem(int slot) {
		return Storage[slot];
	}

	void SetSlot(int slot, ItemStack item) {
		Storage[slot] = item;
	}
	
	int GetSlotCount() {
		return NumOfSlots;
	}

	void ChangeSlotSize(int slotCount) {
		Storage.resize(slotCount);
		NumOfSlots = slotCount;
	}

protected:
	void Initialize(int slotCount) {
		NumOfSlots = slotCount;
		Storage.resize(slotCount);
	}

private:
	
	int NumOfSlots = 64;
	std::vector<ItemStack> Storage;

};
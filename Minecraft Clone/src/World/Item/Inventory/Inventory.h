#pragma once
#include "../../../World/Typenames.h"
#include "../../../World/Chunk/Block/Blocks.h"
#include "../ItemStack.h"
#include <unordered_map>


typedef class Inventory {
public:

	Inventory(int InventorySize = 0xFFFFFFFF) {
		if (InventorySize == 0xFFFFFFFF)
			return;
		Initialize(InventorySize);
	}

	~Inventory() {
		if (Storage != nullptr) {
			//delete[] Storage;
		}
		
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

	int ChangeSlotSize() {
		//do later
	}

protected:
	void Initialize(int slotCount) {
		NumOfSlots = slotCount;
		Storage = new ItemStack[slotCount];
	}

private:
	
	int NumOfSlots = 64;
	ItemStack* Storage;

};
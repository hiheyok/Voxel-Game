#pragma once
#include "Inventory.h"
#include "../../Chunk/Block/Blocks.h"
#include "../../Item/Items.h"
class EntityInventory : public Inventory {
public:
	EntityInventory(int SlotCount = 1) {
		Initialize(SlotCount);
	}

	int RightHandSlot = 0;
	int LeftHandSlot = 0;

private:
};
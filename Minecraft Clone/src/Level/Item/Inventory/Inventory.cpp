#include "Inventory.h"

ItemStack Inventory::GetItem(int slot) {
	return Storage[slot];
}

void Inventory::SetSlot(int slot, ItemStack item) {
	Storage[slot] = item;
}

int Inventory::GetSlotCount() {
	return NumOfSlots;
}

void Inventory::ChangeSlotSize(int slotCount) {
	Storage.resize(slotCount);
	NumOfSlots = slotCount;
}

void Inventory::Initialize(int slotCount) {
	NumOfSlots = slotCount;
	Storage.resize(slotCount);
}
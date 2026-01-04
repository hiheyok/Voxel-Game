// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Item/Inventory/Inventory.h"

#include "Level/Item/ItemStack.h"

ItemStack Inventory::GetItem(int slot) { return storage_[slot]; }

void Inventory::SetSlot(int slot, ItemStack item) { storage_[slot] = item; }

int Inventory::GetSlotCount() { return num_slots_; }

void Inventory::ChangeSlotSize(int slotCount) {
  storage_.resize(slotCount);
  num_slots_ = slotCount;
}

void Inventory::Initialize(int slotCount) {
  num_slots_ = slotCount;
  storage_.resize(slotCount);
}

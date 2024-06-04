#pragma once

#include "Item.h"

class ItemStack { //Contains additional properties of item such as item count enchantments, etc
public:
	ItemStack() {

	}

	ItemStack(Item item) {
		m_item = item;
		Initialized = true;
	}

	bool IsInitialized() {
		return Initialized;
	}

	Item m_item;
	int ItemCount = 1;
private:
	bool Initialized = false;

};
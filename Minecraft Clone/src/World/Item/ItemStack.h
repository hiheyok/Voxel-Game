#pragma once

#include "Item.h"

class ItemStack { //Contains additional properties of item such as item count enchantments, etc
public:
	ItemStack() {

	}

	ItemStack(Item item) {
		m_item = item;
	}

	Item m_item;
	int ItemCount = 1;
private:


};
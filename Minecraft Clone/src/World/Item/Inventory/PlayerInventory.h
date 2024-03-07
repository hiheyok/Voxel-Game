#pragma once

#include "Inventory.h"
#include "../../Chunk/Block/Blocks.h"

class PlayerInventory : public Inventory {
public:
	PlayerInventory() {
		Inventory(Blocks.BlockTypeData.size()); 
	}

private:
};
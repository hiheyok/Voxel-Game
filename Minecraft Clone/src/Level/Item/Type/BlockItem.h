#pragma once

#include "../../Chunk/Block/Blocks.h"
#include "../Item.h"

class BlockItem : public Item {
public:
	BlockItem(BlockID block_) {
		Properties.block = block_;
		Properties.IsBlockItem = true;
	}

private:


};
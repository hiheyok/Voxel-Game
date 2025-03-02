#pragma once

#include "../../Chunk/Block/Blocks.h"
#include "../Item.h"

class BlockItem : public Item {
public:
	BlockItem(BlockID block_) {
		properties_.block_ = block_;
		properties_.is_block_item_ = true;
	}

private:


};
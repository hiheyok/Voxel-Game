#pragma once
#include "../Chunk/Block/Blocks.h"
#include "../Typenames.h"

#include <string>


typedef class Item {
public:
	Item() {

	}

	BlockID getBlock() {
		return Properties.block;
	}

	bool CheckIsBlockItem() {
		return Properties.IsBlockItem;
	}

	struct ItemProperties {
		int MAX_STACK = 64;
		bool IsBlockItem = false;
		BlockID block = NULL;
		ItemID ID = NULL;
		std::string Name = "";
	} Properties;
	
private:

};

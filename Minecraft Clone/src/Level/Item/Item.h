#pragma once
#include "../Chunk/Block/Blocks.h"
#include "../Typenames.h"

#include <string>

struct ItemProperties {
	int MAX_STACK = 64;
	bool IsBlockItem = false;
	BlockID block = NULL;
	ItemID ID = NULL;
	std::string Name = "";
};

class Item {
public:
	Item() {

	}

	const BlockID GetBlock();

	RawTextureData GetTexture();

	bool CheckIsBlockItem();

	ItemProperties Properties;
	
private:

};


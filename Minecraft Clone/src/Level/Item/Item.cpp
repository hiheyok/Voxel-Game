#include "Item.h"

const BlockID Item::GetBlock() {
	return properties_.block_;
}

RawTextureData Item::GetTexture() {
	return RawTextureData();
}

bool Item::CheckIsBlockItem() {
	return properties_.is_block_item_;
}
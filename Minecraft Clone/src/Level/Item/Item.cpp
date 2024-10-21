#include "Item.h"

const BlockID Item::GetBlock() {
	return Properties.block;
}

RawTextureData Item::GetTexture() {
	return RawTextureData();
}

bool Item::CheckIsBlockItem() {
	return Properties.IsBlockItem;
}
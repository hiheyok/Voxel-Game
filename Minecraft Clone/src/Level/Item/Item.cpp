#include "Item.h"

const BlockID Item::GetBlock() {
	return Properties.block;
}

RawTextureData Item::GetTexture() {

}

bool Item::CheckIsBlockItem() {
	return Properties.IsBlockItem;
}
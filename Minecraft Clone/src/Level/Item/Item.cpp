#include "Item.h"
#include "../../RenderEngine/OpenGL/Texture/Texture.h"

BlockID Item::GetBlock() const {
    return properties_.block_;
}

RawTextureData Item::GetTexture() const {
    return RawTextureData();
}

bool Item::CheckIsBlockItem() const {
    return properties_.is_block_item_;
}
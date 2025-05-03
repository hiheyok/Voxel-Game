// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Item/Item.h"

#include "RenderEngine/OpenGL/Texture/Texture.h"

BlockID Item::GetBlock() const { return properties_.block_; }

bool Item::CheckIsBlockItem() const { return properties_.is_block_item_; }

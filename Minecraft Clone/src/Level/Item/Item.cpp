// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Item/Item.h"

#include "Core/Typenames.h"

Item::Item() = default;
Item::~Item() = default;

bool ItemProperties::operator==(const ItemProperties& other) const noexcept =
    default;
bool Item::operator==(const Item& other) const noexcept = default;

BlockID Item::GetBlock() const { return properties_.block_; }

bool Item::CheckIsBlockItem() const { return properties_.is_block_item_; }

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Item/Items.h"

#include "Core/GameContext/GameContext.h"
#include "Core/Typenames.h"
#include "Level/Block/Block.h"
#include "Level/Item/Item.h"
#include "Level/Item/Type/BlockItem.h"
#include "Utils/LogUtils.h"

ItemList::ItemList(GameContext& context) : context_{context} {}
ItemList::~ItemList() = default;

Item ItemList::RegisterBlockItem(BlockID block) {
  BlockItem item{block};
  item.properties_.is_block_item_ = true;
  item.properties_.block_ = block;
  item.properties_.id_ = item_count_;
  item.properties_.name_ = context_.blocks_->GetBlockType(block)->block_name_;

  item_container_[item_count_] = item;
  block_item_ids_[block] = item_count_;

  item_count_++;

  LOG_INFO("Registered Block Item: {} | Name: {}", item_count_,
           item.properties_.name_);

  return item;
}

int ItemList::GetTotalItemCount() { return item_count_; }

Item ItemList::GetItem(ItemID id) { return item_container_[id]; }

ItemID ItemList::GetBlockItem(BlockID block) { return block_item_ids_[block]; }

void ItemList::RegisterAll() {
  for (BlockID b = 0; b < context_.blocks_->block_type_data_.size(); b++) {
    if (block_item_ids_.count(b)) continue;
    RegisterBlockItem(b);
  }
}

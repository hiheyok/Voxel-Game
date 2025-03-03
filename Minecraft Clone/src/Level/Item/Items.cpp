#include "Items.h"
#include "Type/BlockItem.h"
#include "../../Utils/LogUtils.h"

Item ItemList::RegisterBlockItem(BlockID block) {
	BlockItem item{ block };
	item.properties_.is_block_item_ = true;
	item.properties_.block_ = block;
	item.properties_.id_ = item_count_;
	item.properties_.name_ = g_blocks.GetBlockType(block)->block_name_;

	item_container_[item_count_] = item;
	block_item_ids_[block] = item_count_;

	item_count_++;
	
	g_logger.LogInfo("Item", "Registered Block Item: " + std::to_string(item_count_) + " | Name: " + item.properties_.name_);

	return item;
}

int ItemList::GetTotalItemCount() {
	return item_count_;
}

Item ItemList::GetItem(ItemID id) {
	return item_container_[id];
}

ItemID ItemList::GetBlockItem(BlockID block) {
	return block_item_ids_[block];
}

void ItemList::RegisterAll() {
	for (BlockID b = 0; b < g_blocks.block_type_data_.size(); b++) {
		if (block_item_ids_.count(b)) continue;

		RegisterBlockItem(b);
	}
}
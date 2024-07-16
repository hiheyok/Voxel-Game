#include "Items.h"
#include "Type/BlockItem.h"
#include "../../Utils/LogUtils.h"

Item ItemList::RegisterBlockItem(BlockID block) {
	BlockItem item(block);
	item.Properties.IsBlockItem = true;
	item.Properties.block = block;
	item.Properties.ID = ItemCount;
	item.Properties.Name = Blocks.getBlockType(block)->BlockName;

	ItemContainer[ItemCount] = item;
	BlockItemIDs[block] = ItemCount;

	ItemCount++;
	
	Logger.LogInfo("Item", "Registered Block Item: " + std::to_string(ItemCount) + " | Name: " + item.Properties.Name);

	return item;
}

int ItemList::GetTotalItemCount() {
	return ItemCount;
}

Item ItemList::GetItem(ItemID id) {
	return ItemContainer[id];
}

ItemID ItemList::GetBlockItem(BlockID block) {
	return BlockItemIDs[block];
}

void ItemList::REGISTER_ALL() {
	for (BlockID b = 0; b < Blocks.BlockTypeData.size(); b++) {
		if (BlockItemIDs.count(b)) continue;

		RegisterBlockItem(b);
	}
}
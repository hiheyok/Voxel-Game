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

	ItemCount++;
	
	Logger.LogInfo("Item", "Registered Block Item: " + std::to_string(ItemCount) + " | Name: " + item.Properties.Name);

	return item;
}

int ItemList::GetTotalItemCount() {
	return ItemCount;
}
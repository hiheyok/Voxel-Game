#pragma once
#include "Item.h"
#include "../Chunk/Block/Blocks.h"


class ItemList{
private:

	Item RegisterBlockItem(BlockID block);

	int ItemCount = 0;
	std::unordered_map<ItemID, Item> ItemContainer;
	std::unordered_map<BlockID, ItemID> BlockItemIDs;

public:
	Item AIR = RegisterBlockItem(Blocks.AIR);
	Item STONE = RegisterBlockItem(Blocks.STONE);
	Item DIRT = RegisterBlockItem(Blocks.DIRT);
	Item WATER = RegisterBlockItem(Blocks.WATER);
	Item LAVA = RegisterBlockItem(Blocks.LAVA);
	Item GRASS = RegisterBlockItem(Blocks.GRASS);
	Item SAND = RegisterBlockItem(Blocks.SAND);
	Item OAK_LOG = RegisterBlockItem(Blocks.OAK_LOG);
	Item OAK_LEAF = RegisterBlockItem(Blocks.OAK_LEAF);
	Item DIAMOND_BLOCK = RegisterBlockItem(Blocks.DIAMOND_BLOCK);
	Item OAK_PLANK = RegisterBlockItem(Blocks.OAK_PLANK);
	Item COBBLESTONE = RegisterBlockItem(Blocks.COBBLESTONE);
	Item BRICK = RegisterBlockItem(Blocks.BRICK);
	Item WHITE_CONCRETE = RegisterBlockItem(Blocks.WHITE_CONCRETE);
	Item BLUE_STAINED_GLASS = RegisterBlockItem(Blocks.BLUE_STAINED_GLASS);
	Item GREEN_STAINED_GLASS = RegisterBlockItem(Blocks.GREEN_STAINED_GLASS);
	Item BLACK_STAINED_GLASS = RegisterBlockItem(Blocks.BLACK_STAINED_GLASS);
	Item BROWN_STAINED_GLASS = RegisterBlockItem(Blocks.BROWN_STAINED_GLASS);
	Item CYAN_STAINED_GLASS = RegisterBlockItem(Blocks.CYAN_STAINED_GLASS);
	Item GRAY_STAINED_GLASS = RegisterBlockItem(Blocks.GRAY_STAINED_GLASS);
	Item LIGHT_BLUE_STAINED_GLASS = RegisterBlockItem(Blocks.LIGHT_BLUE_STAINED_GLASS);
	Item LIGHT_GRAY_STAINED_GLASS = RegisterBlockItem(Blocks.LIGHT_GRAY_STAINED_GLASS);
	Item LIME_STAINED_GLASS = RegisterBlockItem(Blocks.LIME_STAINED_GLASS);
	Item MAGENTA_STAINED_GLASS = RegisterBlockItem(Blocks.MAGENTA_STAINED_GLASS);
	Item AIORANGE_STAINED_GLASSR = RegisterBlockItem(Blocks.ORANGE_STAINED_GLASS);
	Item PINK_STAINED_GLASS = RegisterBlockItem(Blocks.PINK_STAINED_GLASS);
	Item PURPLE_STAINED_GLASS = RegisterBlockItem(Blocks.PURPLE_STAINED_GLASS);
	Item RED_STAINED_GLASS = RegisterBlockItem(Blocks.RED_STAINED_GLASS);
	Item WHITE_STAINED_GLASS = RegisterBlockItem(Blocks.WHITE_STAINED_GLASS);
	Item YELLOW_STAINED_GLASS = RegisterBlockItem(Blocks.YELLOW_STAINED_GLASS);

	Item GetItem(ItemID id) {
		return ItemContainer[id];
	}

	ItemID GetBlockItem(BlockID block) {
		return BlockItemIDs[block];
	}

	int GetTotalItemCount();
};

extern ItemList Items;

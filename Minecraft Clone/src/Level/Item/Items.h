// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Block/Blocks.h"
#include "Level/Item/Item.h"

class ItemList {
 private:
  Item RegisterBlockItem(BlockID block);

  int item_count_ = 0;

  FastHashMap<BlockID, ItemID> block_item_ids_;

 public:
  FastHashMap<ItemID, Item> item_container_;

  Item AIR = RegisterBlockItem(g_blocks.AIR);
  Item STONE = RegisterBlockItem(g_blocks.STONE);
  Item DIRT = RegisterBlockItem(g_blocks.DIRT);
  Item WATER = RegisterBlockItem(g_blocks.WATER);
  Item LAVA = RegisterBlockItem(g_blocks.LAVA);
  Item GRASS = RegisterBlockItem(g_blocks.GRASS);
  Item SAND = RegisterBlockItem(g_blocks.SAND);
  Item OAK_LOG = RegisterBlockItem(g_blocks.OAK_LOG);
  Item STRIPPED_OAK_LOG = RegisterBlockItem(g_blocks.STRIPPED_OAK_LOG);
  Item OAK_LEAF = RegisterBlockItem(g_blocks.OAK_LEAF);
  Item OAK_PLANKS = RegisterBlockItem(g_blocks.OAK_PLANKS);
  Item BAMBOO_PLANKS = RegisterBlockItem(g_blocks.BAMBOO_PLANKS);
  Item ACACIA_PLANKS = RegisterBlockItem(g_blocks.ACACIA_PLANKS);
  Item BIRCH_PLANKS = RegisterBlockItem(g_blocks.BIRCH_PLANKS);
  Item CHERRY_PLANKS = RegisterBlockItem(g_blocks.CHERRY_PLANKS);
  Item CRIMSON_PLANKS = RegisterBlockItem(g_blocks.CRIMSON_PLANKS);
  Item DARK_OAK_PLANKS = RegisterBlockItem(g_blocks.DARK_OAK_PLANKS);
  Item JUNGLE_PLANKS = RegisterBlockItem(g_blocks.JUNGLE_PLANKS);
  Item MANGROVE_PLANKS = RegisterBlockItem(g_blocks.MANGROVE_PLANKS);
  Item SPRUCE_PLANKS = RegisterBlockItem(g_blocks.SPRUCE_PLANKS);
  Item WARPED_PLANKS = RegisterBlockItem(g_blocks.WARPED_PLANKS);
  Item OAK_WOOD = RegisterBlockItem(g_blocks.OAK_WOOD);
  Item ACACIA_WOOD = RegisterBlockItem(g_blocks.ACACIA_WOOD);
  Item BIRCH_WOOD = RegisterBlockItem(g_blocks.BIRCH_WOOD);
  Item CHERRY_WOOD = RegisterBlockItem(g_blocks.CHERRY_WOOD);
  Item DARK_OAK_WOOD = RegisterBlockItem(g_blocks.DARK_OAK_WOOD);
  Item JUNGLE_WOOD = RegisterBlockItem(g_blocks.JUNGLE_WOOD);
  Item MANGROVE_WOOD = RegisterBlockItem(g_blocks.MANGROVE_WOOD);
  Item SPRUCE_WOOD = RegisterBlockItem(g_blocks.SPRUCE_WOOD);
  Item STRIPPED_OAK_WOOD = RegisterBlockItem(g_blocks.STRIPPED_OAK_WOOD);
  Item STRIPPED_ACACIA_WOOD = RegisterBlockItem(g_blocks.STRIPPED_ACACIA_WOOD);
  Item STRIPPED_BIRCH_WOOD = RegisterBlockItem(g_blocks.STRIPPED_BIRCH_WOOD);
  Item STRIPPED_CHERRY_WOOD = RegisterBlockItem(g_blocks.STRIPPED_CHERRY_WOOD);
  Item STRIPPED_DARK_OAK_WOOD =
      RegisterBlockItem(g_blocks.STRIPPED_DARK_OAK_WOOD);
  Item STRIPPED_JUNGLE_WOOD = RegisterBlockItem(g_blocks.STRIPPED_JUNGLE_WOOD);
  Item STRIPPED_MANGROVE_WOOD =
      RegisterBlockItem(g_blocks.STRIPPED_MANGROVE_WOOD);
  Item STRIPPED_SPRUCE_WOOD = RegisterBlockItem(g_blocks.STRIPPED_SPRUCE_WOOD);
  Item BRICKS = RegisterBlockItem(g_blocks.BRICKS);
  Item STONE_BRICKS = RegisterBlockItem(g_blocks.STONE_BRICKS);
  Item CRACKED_STONE_BRICKS = RegisterBlockItem(g_blocks.CRACKED_STONE_BRICKS);
  Item CHISELED_STONE_BRICKS =
      RegisterBlockItem(g_blocks.CHISELED_STONE_BRICKS);
  Item MOSSY_STONE_BRICKS = RegisterBlockItem(g_blocks.MOSSY_STONE_BRICKS);
  Item DEEPSLATE_BRICKS = RegisterBlockItem(g_blocks.DEEPSLATE_BRICKS);
  Item CRACKED_DEEPSLATE_BRICKS =
      RegisterBlockItem(g_blocks.CRACKED_DEEPSLATE_BRICKS);
  Item TUFF_BRICKS = RegisterBlockItem(g_blocks.TUFF_BRICKS);
  Item CHISELED_TUFF_BRICKS = RegisterBlockItem(g_blocks.CHISELED_TUFF_BRICKS);
  Item MUD_BRICKS = RegisterBlockItem(g_blocks.MUD_BRICKS);
  Item PRISMARINE_BRICKS = RegisterBlockItem(g_blocks.PRISMARINE_BRICKS);
  Item NETHER_BRICKS = RegisterBlockItem(g_blocks.NETHER_BRICKS);
  Item CRACKED_NETHER_BRICKS =
      RegisterBlockItem(g_blocks.CRACKED_NETHER_BRICKS);
  Item CHISELED_NETHER_BRICKS =
      RegisterBlockItem(g_blocks.CHISELED_NETHER_BRICKS);
  Item RED_NETHER_BRICKS = RegisterBlockItem(g_blocks.RED_NETHER_BRICKS);
  Item POLISHED_BLACKSTONE_BRICK =
      RegisterBlockItem(g_blocks.POLISHED_BLACKSTONE_BRICK);
  Item CRACKED_POLISHED_BLACKSTONE_BRICK =
      RegisterBlockItem(g_blocks.CRACKED_POLISHED_BLACKSTONE_BRICK);
  Item END_STONE_BRICKS = RegisterBlockItem(g_blocks.END_STONE_BRICKS);
  Item COBBLESTONE = RegisterBlockItem(g_blocks.COBBLESTONE);
  Item BLUE_STAINED_GLASS = RegisterBlockItem(g_blocks.BLUE_STAINED_GLASS);
  Item GREEN_STAINED_GLASS = RegisterBlockItem(g_blocks.GREEN_STAINED_GLASS);
  Item BLACK_STAINED_GLASS = RegisterBlockItem(g_blocks.BLACK_STAINED_GLASS);
  Item BROWN_STAINED_GLASS = RegisterBlockItem(g_blocks.BROWN_STAINED_GLASS);
  Item CYAN_STAINED_GLASS = RegisterBlockItem(g_blocks.CYAN_STAINED_GLASS);
  Item GRAY_STAINED_GLASS = RegisterBlockItem(g_blocks.GRAY_STAINED_GLASS);
  Item LIGHT_BLUE_STAINED_GLASS =
      RegisterBlockItem(g_blocks.LIGHT_BLUE_STAINED_GLASS);
  Item LIGHT_GRAY_STAINED_GLASS =
      RegisterBlockItem(g_blocks.LIGHT_GRAY_STAINED_GLASS);
  Item LIME_STAINED_GLASS = RegisterBlockItem(g_blocks.LIME_STAINED_GLASS);
  Item MAGENTA_STAINED_GLASS =
      RegisterBlockItem(g_blocks.MAGENTA_STAINED_GLASS);
  Item AIORANGE_STAINED_GLASSR =
      RegisterBlockItem(g_blocks.ORANGE_STAINED_GLASS);
  Item PINK_STAINED_GLASS = RegisterBlockItem(g_blocks.PINK_STAINED_GLASS);
  Item PURPLE_STAINED_GLASS = RegisterBlockItem(g_blocks.PURPLE_STAINED_GLASS);
  Item RED_STAINED_GLASS = RegisterBlockItem(g_blocks.RED_STAINED_GLASS);
  Item WHITE_STAINED_GLASS = RegisterBlockItem(g_blocks.WHITE_STAINED_GLASS);
  Item YELLOW_STAINED_GLASS = RegisterBlockItem(g_blocks.YELLOW_STAINED_GLASS);
  Item WHITE_WOOL = RegisterBlockItem(g_blocks.WHITE_WOOL);
  Item BLACK_WOOL = RegisterBlockItem(g_blocks.BLACK_WOOL);
  Item BLUE_WOOL = RegisterBlockItem(g_blocks.BLUE_WOOL);
  Item BROWN_WOOL = RegisterBlockItem(g_blocks.BROWN_WOOL);
  Item CYAN_WOOL = RegisterBlockItem(g_blocks.CYAN_WOOL);
  Item GRAY_WOOL = RegisterBlockItem(g_blocks.GRAY_WOOL);
  Item GREEN_WOOL = RegisterBlockItem(g_blocks.GREEN_WOOL);
  Item LIGHT_BLUE_WOOL = RegisterBlockItem(g_blocks.LIGHT_BLUE_WOOL);
  Item LIGHT_GRAY_WOOL = RegisterBlockItem(g_blocks.LIGHT_GRAY_WOOL);
  Item LIME_WOOL = RegisterBlockItem(g_blocks.LIME_WOOL);
  Item MAGENTA_WOOL = RegisterBlockItem(g_blocks.MAGENTA_WOOL);
  Item ORANGE_WOOL = RegisterBlockItem(g_blocks.ORANGE_WOOL);
  Item PINK_WOOL = RegisterBlockItem(g_blocks.PINK_WOOL);
  Item PURPLE_WOOL = RegisterBlockItem(g_blocks.PURPLE_WOOL);
  Item RED_WOOL = RegisterBlockItem(g_blocks.RED_WOOL);
  Item YELLOW_WOOL = RegisterBlockItem(g_blocks.YELLOW_WOOL);
  Item WHITE_TERRACOTTA = RegisterBlockItem(g_blocks.WHITE_TERRACOTTA);
  Item BLACK_TERRACOTTA = RegisterBlockItem(g_blocks.BLACK_TERRACOTTA);
  Item BLUE_TERRACOTTA = RegisterBlockItem(g_blocks.BLUE_TERRACOTTA);
  Item BROWN_TERRACOTTA = RegisterBlockItem(g_blocks.BROWN_TERRACOTTA);
  Item CYAN_TERRACOTTA = RegisterBlockItem(g_blocks.CYAN_TERRACOTTA);
  Item GRAY_TERRACOTTA = RegisterBlockItem(g_blocks.GRAY_TERRACOTTA);
  Item GREEN_TERRACOTTA = RegisterBlockItem(g_blocks.GREEN_TERRACOTTA);
  Item LIGHT_BLUE_TERRACOTTA =
      RegisterBlockItem(g_blocks.LIGHT_BLUE_TERRACOTTA);
  Item LIGHT_GRAY_TERRACOTTA =
      RegisterBlockItem(g_blocks.LIGHT_GRAY_TERRACOTTA);
  Item LIME_TERRACOTTA = RegisterBlockItem(g_blocks.LIME_TERRACOTTA);
  Item MAGENTA_TERRACOTTA = RegisterBlockItem(g_blocks.MAGENTA_TERRACOTTA);
  Item ORANGE_TERRACOTTA = RegisterBlockItem(g_blocks.ORANGE_TERRACOTTA);
  Item PINK_TERRACOTTA = RegisterBlockItem(g_blocks.PINK_TERRACOTTA);
  Item PURPLE_TERRACOTTA = RegisterBlockItem(g_blocks.PURPLE_TERRACOTTA);
  Item RED_TERRACOTTA = RegisterBlockItem(g_blocks.RED_TERRACOTTA);
  Item YELLOW_TERRACOTTA = RegisterBlockItem(g_blocks.YELLOW_TERRACOTTA);
  Item WHITE_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.WHITE_GLAZED_TERRACOTTA);
  Item BLACK_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.BLACK_GLAZED_TERRACOTTA);
  Item BLUE_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.BLUE_GLAZED_TERRACOTTA);
  Item BROWN_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.BROWN_GLAZED_TERRACOTTA);
  Item CYAN_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.CYAN_GLAZED_TERRACOTTA);
  Item GRAY_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.GRAY_GLAZED_TERRACOTTA);
  Item GREEN_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.GREEN_GLAZED_TERRACOTTA);
  Item LIGHT_BLUE_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.LIGHT_BLUE_GLAZED_TERRACOTTA);
  Item LIGHT_GRAY_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.LIGHT_GRAY_GLAZED_TERRACOTTA);
  Item LIME_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.LIME_GLAZED_TERRACOTTA);
  Item MAGENTA_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.MAGENTA_GLAZED_TERRACOTTA);
  Item ORANGE_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.ORANGE_GLAZED_TERRACOTTA);
  Item PINK_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.PINK_GLAZED_TERRACOTTA);
  Item PURPLE_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.PURPLE_GLAZED_TERRACOTTA);
  Item RED_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.RED_GLAZED_TERRACOTTA);
  Item YELLOW_GLAZED_TERRACOTTA =
      RegisterBlockItem(g_blocks.YELLOW_GLAZED_TERRACOTTA);
  Item TERRACOTTA = RegisterBlockItem(g_blocks.TERRACOTTA);
  Item WHITE_CONCRETE = RegisterBlockItem(g_blocks.WHITE_CONCRETE);
  Item BLACK_CONCRETE = RegisterBlockItem(g_blocks.BLACK_CONCRETE);
  Item BLUE_CONCRETE = RegisterBlockItem(g_blocks.BLUE_CONCRETE);
  Item BROWN_CONCRETE = RegisterBlockItem(g_blocks.BROWN_CONCRETE);
  Item CYAN_CONCRETE = RegisterBlockItem(g_blocks.CYAN_CONCRETE);
  Item GRAY_CONCRETE = RegisterBlockItem(g_blocks.GRAY_CONCRETE);
  Item GREEN_CONCRETE = RegisterBlockItem(g_blocks.GREEN_CONCRETE);
  Item LIGHT_BLUE_CONCRETE = RegisterBlockItem(g_blocks.LIGHT_BLUE_CONCRETE);
  Item LIGHT_GRAY_CONCRETE = RegisterBlockItem(g_blocks.LIGHT_GRAY_CONCRETE);
  Item LIME_CONCRETE = RegisterBlockItem(g_blocks.LIME_CONCRETE);
  Item MAGENTA_CONCRETE = RegisterBlockItem(g_blocks.MAGENTA_CONCRETE);
  Item ORANGE_CONCRETE = RegisterBlockItem(g_blocks.ORANGE_CONCRETE);
  Item PINK_CONCRETE = RegisterBlockItem(g_blocks.PINK_CONCRETE);
  Item PURPLE_CONCRETE = RegisterBlockItem(g_blocks.PURPLE_CONCRETE);
  Item RED_CONCRETE = RegisterBlockItem(g_blocks.RED_CONCRETE);
  Item YELLOW_CONCRETE = RegisterBlockItem(g_blocks.YELLOW_CONCRETE);
  Item WHITE_CONCRETE_POWDER =
      RegisterBlockItem(g_blocks.WHITE_CONCRETE_POWDER);
  Item BLACK_CONCRETE_POWDER =
      RegisterBlockItem(g_blocks.BLACK_CONCRETE_POWDER);
  Item BLUE_CONCRETE_POWDER = RegisterBlockItem(g_blocks.BLUE_CONCRETE_POWDER);
  Item BROWN_CONCRETE_POWDER =
      RegisterBlockItem(g_blocks.BROWN_CONCRETE_POWDER);
  Item CYAN_CONCRETE_POWDER = RegisterBlockItem(g_blocks.CYAN_CONCRETE_POWDER);
  Item GRAY_CONCRETE_POWDER = RegisterBlockItem(g_blocks.GRAY_CONCRETE_POWDER);
  Item GREEN_CONCRETE_POWDER =
      RegisterBlockItem(g_blocks.GREEN_CONCRETE_POWDER);
  Item LIGHT_BLUE_CONCRETE_POWDER =
      RegisterBlockItem(g_blocks.LIGHT_BLUE_CONCRETE_POWDER);
  Item LIGHT_GRAY_CONCRETE_POWDER =
      RegisterBlockItem(g_blocks.LIGHT_GRAY_CONCRETE_POWDER);
  Item LIME_CONCRETE_POWDER = RegisterBlockItem(g_blocks.LIME_CONCRETE_POWDER);
  Item MAGENTA_CONCRETE_POWDER =
      RegisterBlockItem(g_blocks.MAGENTA_CONCRETE_POWDER);
  Item ORANGE_CONCRETE_POWDER =
      RegisterBlockItem(g_blocks.ORANGE_CONCRETE_POWDER);
  Item PINK_CONCRETE_POWDER = RegisterBlockItem(g_blocks.PINK_CONCRETE_POWDER);
  Item PURPLE_CONCRETE_POWDER =
      RegisterBlockItem(g_blocks.PURPLE_CONCRETE_POWDER);
  Item RED_CONCRETE_POWDER = RegisterBlockItem(g_blocks.RED_CONCRETE_POWDER);
  Item YELLOW_CONCRETE_POWDER =
      RegisterBlockItem(g_blocks.YELLOW_CONCRETE_POWDER);
  Item PODZOL = RegisterBlockItem(g_blocks.PODZOL);
  Item MYCELIUM = RegisterBlockItem(g_blocks.MYCELIUM);
  Item DIRT_PATH = RegisterBlockItem(g_blocks.DIRT_PATH);
  Item COARSE_DIRT = RegisterBlockItem(g_blocks.COARSE_DIRT);
  Item ROOTED_DIRT = RegisterBlockItem(g_blocks.ROOTED_DIRT);
  Item FARMLAND = RegisterBlockItem(g_blocks.FARMLAND);
  Item FARMLAND_MOIST = RegisterBlockItem(g_blocks.FARMLAND_MOIST);
  Item MUD = RegisterBlockItem(g_blocks.MUD);
  Item CLAY = RegisterBlockItem(g_blocks.CLAY);
  Item GRAVEL = RegisterBlockItem(g_blocks.GRAVEL);
  Item RED_SAND = RegisterBlockItem(g_blocks.RED_SAND);
  Item ICE = RegisterBlockItem(g_blocks.ICE);
  Item BLUE_ICE = RegisterBlockItem(g_blocks.BLUE_ICE);
  Item SNOW_BLOCK = RegisterBlockItem(g_blocks.SNOW_BLOCK);
  Item MOSS_BLOCK = RegisterBlockItem(g_blocks.MOSS_BLOCK);
  Item GRANITE = RegisterBlockItem(g_blocks.GRANITE);
  Item DIORITE = RegisterBlockItem(g_blocks.DIORITE);
  Item ANDESITE = RegisterBlockItem(g_blocks.ANDESITE);
  Item CALCITE = RegisterBlockItem(g_blocks.CALCITE);
  Item DRIPSTONE_BLOCK = RegisterBlockItem(g_blocks.DRIPSTONE_BLOCK);
  Item PRISMARINE = RegisterBlockItem(g_blocks.PRISMARINE);
  Item MAGMA = RegisterBlockItem(g_blocks.MAGMA);
  Item OBSIDIAN = RegisterBlockItem(g_blocks.OBSIDIAN);
  Item CRYING_OBSIDIAN = RegisterBlockItem(g_blocks.CRYING_OBSIDIAN);
  Item NETHERRACK = RegisterBlockItem(g_blocks.NETHERRACK);
  Item CRIMSON_NYLIUM = RegisterBlockItem(g_blocks.CRIMSON_NYLIUM);
  Item WARPED_NYLIUM = RegisterBlockItem(g_blocks.WARPED_NYLIUM);
  Item SOUL_SAND = RegisterBlockItem(g_blocks.SOUL_SAND);
  Item SOUL_SOIL = RegisterBlockItem(g_blocks.SOUL_SOIL);
  Item BONE_BLOCK = RegisterBlockItem(g_blocks.BONE_BLOCK);
  Item BLACKSTONE = RegisterBlockItem(g_blocks.BLACKSTONE);
  Item BASALT = RegisterBlockItem(g_blocks.BASALT);
  Item SMOOTH_BASALT = RegisterBlockItem(g_blocks.SMOOTH_BASALT);
  Item END_STONE = RegisterBlockItem(g_blocks.END_STONE);
  Item COAL_ORE = RegisterBlockItem(g_blocks.COAL_ORE);
  Item DEEPSLATE_COAL_ORE = RegisterBlockItem(g_blocks.DEEPSLATE_COAL_ORE);
  Item IRON_ORE = RegisterBlockItem(g_blocks.IRON_ORE);
  Item DEEPSLATE_IRON_ORE = RegisterBlockItem(g_blocks.DEEPSLATE_IRON_ORE);
  Item COPPER_ORE = RegisterBlockItem(g_blocks.COPPER_ORE);
  Item DEEPSLATE_COPPER_ORE = RegisterBlockItem(g_blocks.DEEPSLATE_COPPER_ORE);
  Item GOLD_ORE = RegisterBlockItem(g_blocks.GOLD_ORE);
  Item DEEPSLATE_GOLD_ORE = RegisterBlockItem(g_blocks.DEEPSLATE_GOLD_ORE);
  Item REDSTONE_ORE = RegisterBlockItem(g_blocks.REDSTONE_ORE);
  Item DEEPSLATE_REDSTONE_ORE =
      RegisterBlockItem(g_blocks.DEEPSLATE_REDSTONE_ORE);
  Item EMERALD_ORE = RegisterBlockItem(g_blocks.EMERALD_ORE);
  Item DEEPSLATE_EMERALD_ORE =
      RegisterBlockItem(g_blocks.DEEPSLATE_EMERALD_ORE);
  Item LAPIS_ORE = RegisterBlockItem(g_blocks.LAPIS_ORE);
  Item DEEPSLATE_LAPIS_ORE = RegisterBlockItem(g_blocks.DEEPSLATE_LAPIS_ORE);
  Item DIAMOND_ORE = RegisterBlockItem(g_blocks.DIAMOND_ORE);
  Item DEEPSLATE_DIAMOND_ORE =
      RegisterBlockItem(g_blocks.DEEPSLATE_DIAMOND_ORE);
  Item NETHER_GOLD_ORE = RegisterBlockItem(g_blocks.NETHER_GOLD_ORE);
  Item NETHER_QUARTZ_ORE = RegisterBlockItem(g_blocks.NETHER_QUARTZ_ORE);
  Item ANCIENT_DEBRIS = RegisterBlockItem(g_blocks.ANCIENT_DEBRIS);
  Item RAW_IRON_BLOCK = RegisterBlockItem(g_blocks.RAW_IRON_BLOCK);
  Item RAW_COPPER_BLOCK = RegisterBlockItem(g_blocks.RAW_COPPER_BLOCK);
  Item RAW_GOLD_BLOCK = RegisterBlockItem(g_blocks.RAW_GOLD_BLOCK);
  Item GLOWSTONE = RegisterBlockItem(g_blocks.GLOWSTONE);
  Item AMETHYST_BLOCK = RegisterBlockItem(g_blocks.AMETHYST_BLOCK);
  Item BUDDING_AMETHYST = RegisterBlockItem(g_blocks.BUDDING_AMETHYST);
  Item SPRUCE_LOG = RegisterBlockItem(g_blocks.SPRUCE_LOG);
  Item STRIPPED_SPRUCE_LOG = RegisterBlockItem(g_blocks.STRIPPED_SPRUCE_LOG);
  Item BIRCH_LOG = RegisterBlockItem(g_blocks.BIRCH_LOG);
  Item STRIPPED_BIRCH_LOG = RegisterBlockItem(g_blocks.STRIPPED_BIRCH_LOG);
  Item JUNGLE_LOG = RegisterBlockItem(g_blocks.JUNGLE_LOG);
  Item STRIPPED_JUNGLE_LOG = RegisterBlockItem(g_blocks.STRIPPED_JUNGLE_LOG);
  Item ACACIA_LOG = RegisterBlockItem(g_blocks.ACACIA_LOG);
  Item STRIPPED_ACACIA_LOG = RegisterBlockItem(g_blocks.STRIPPED_ACACIA_LOG);
  Item DARK_OAK_LOG = RegisterBlockItem(g_blocks.DARK_OAK_LOG);
  Item STRIPPED_DARK_OAK_LOG =
      RegisterBlockItem(g_blocks.STRIPPED_DARK_OAK_LOG);
  Item CHERRY_LOG = RegisterBlockItem(g_blocks.CHERRY_LOG);
  Item STRIPPED_CHERRY_LOG = RegisterBlockItem(g_blocks.STRIPPED_CHERRY_LOG);
  Item MANGROVE_LOG = RegisterBlockItem(g_blocks.MANGROVE_LOG);
  Item STRIPPED_MANGROVE_LOG =
      RegisterBlockItem(g_blocks.STRIPPED_MANGROVE_LOG);
  Item CRIMSON_STEM = RegisterBlockItem(g_blocks.CRIMSON_STEM);
  Item STRIPPED_CRIMSON_STEM =
      RegisterBlockItem(g_blocks.STRIPPED_CRIMSON_STEM);
  Item WARPED_STEM = RegisterBlockItem(g_blocks.WARPED_STEM);
  Item STRIPPED_WARPED_STEM = RegisterBlockItem(g_blocks.STRIPPED_WARPED_STEM);
  Item MANGROVE_ROOTS = RegisterBlockItem(g_blocks.MANGROVE_ROOTS);
  Item MUDDY_MANGROVE_ROOTS = RegisterBlockItem(g_blocks.MUDDY_MANGROVE_ROOTS);
  Item MUSHROOM_STEM = RegisterBlockItem(g_blocks.MUSHROOM_STEM);
  Item SPRUCE_LEAVES = RegisterBlockItem(g_blocks.SPRUCE_LEAVES);
  Item BIRCH_LEAVES = RegisterBlockItem(g_blocks.BIRCH_LEAVES);
  Item JUNGLE_LEAVES = RegisterBlockItem(g_blocks.JUNGLE_LEAVES);
  Item ACACIA_LEAVES = RegisterBlockItem(g_blocks.ACACIA_LEAVES);
  Item DARK_OAK_LEAVES = RegisterBlockItem(g_blocks.DARK_OAK_LEAVES);
  Item MANGROVE_LEAVES = RegisterBlockItem(g_blocks.MANGROVE_LEAVES);
  Item CHERRY_LEAVES = RegisterBlockItem(g_blocks.CHERRY_LEAVES);
  Item AZALEA_LEAVES = RegisterBlockItem(g_blocks.AZALEA_LEAVES);
  Item FLOWERING_AZALEA_LEAVES =
      RegisterBlockItem(g_blocks.FLOWERING_AZALEA_LEAVES);
  Item BROWN_MUSHROOM_BLOCK = RegisterBlockItem(g_blocks.BROWN_MUSHROOM_BLOCK);
  Item RED_MUSHROOM_BLOCK = RegisterBlockItem(g_blocks.RED_MUSHROOM_BLOCK);
  Item NETHER_WART_BLOCK = RegisterBlockItem(g_blocks.NETHER_WART_BLOCK);
  Item WARPED_WART_BLOCK = RegisterBlockItem(g_blocks.WARPED_WART_BLOCK);
  Item SHROOMLIGHT = RegisterBlockItem(g_blocks.SHROOMLIGHT);
  Item DRIED_KELP_BLOCK = RegisterBlockItem(g_blocks.DRIED_KELP_BLOCK);
  Item TUBE_CORAL_BLOCK = RegisterBlockItem(g_blocks.TUBE_CORAL_BLOCK);
  Item BRAIN_CORAL_BLOCK = RegisterBlockItem(g_blocks.BRAIN_CORAL_BLOCK);
  Item BUBBLE_CORAL_BLOCK = RegisterBlockItem(g_blocks.BUBBLE_CORAL_BLOCK);
  Item FIRE_CORAL_BLOCK = RegisterBlockItem(g_blocks.FIRE_CORAL_BLOCK);
  Item HORN_CORAL_BLOCK = RegisterBlockItem(g_blocks.HORN_CORAL_BLOCK);
  Item DEAD_TUBE_CORAL_BLOCK =
      RegisterBlockItem(g_blocks.DEAD_TUBE_CORAL_BLOCK);
  Item DEAD_BRAIN_CORAL_BLOCK =
      RegisterBlockItem(g_blocks.DEAD_BRAIN_CORAL_BLOCK);
  Item DEAD_BUBBLE_CORAL_BLOCK =
      RegisterBlockItem(g_blocks.DEAD_BUBBLE_CORAL_BLOCK);
  Item DEAD_FIRE_CORAL_BLOCK =
      RegisterBlockItem(g_blocks.DEAD_FIRE_CORAL_BLOCK);
  Item DEAD_HORN_CORAL_BLOCK =
      RegisterBlockItem(g_blocks.DEAD_HORN_CORAL_BLOCK);
  Item SPONGE = RegisterBlockItem(g_blocks.SPONGE);
  Item WET_SPONGE = RegisterBlockItem(g_blocks.WET_SPONGE);
  Item MELON = RegisterBlockItem(g_blocks.MELON);
  Item PUMPKIN = RegisterBlockItem(g_blocks.PUMPKIN);
  Item CARVED_PUMPKIN = RegisterBlockItem(g_blocks.CARVED_PUMPKIN);
  Item JACK_O_LANTERN = RegisterBlockItem(g_blocks.JACK_O_LANTERN);
  Item HAY_BALE = RegisterBlockItem(g_blocks.HAY_BALE);
  Item BEE_NEST = RegisterBlockItem(g_blocks.BEE_NEST);
  Item HONEYCOMB_BLOCK = RegisterBlockItem(g_blocks.HONEYCOMB_BLOCK);
  Item OCHRE_FROGLIGHT = RegisterBlockItem(g_blocks.OCHRE_FROGLIGHT);
  Item VERDANT_FROGLIGHT = RegisterBlockItem(g_blocks.VERDANT_FROGLIGHT);
  Item PEARLESCENT_FROGLIGHT =
      RegisterBlockItem(g_blocks.PEARLESCENT_FROGLIGHT);
  Item SCULK = RegisterBlockItem(g_blocks.SCULK);
  Item SCULK_CATALYST = RegisterBlockItem(g_blocks.SCULK_CATALYST);
  Item BEDROCK = RegisterBlockItem(g_blocks.BEDROCK);
  Item SEA_LANTERN = RegisterBlockItem(g_blocks.SEA_LANTERN);
  Item REDSTONE_LAMP = RegisterBlockItem(g_blocks.REDSTONE_LAMP);
  Item COPPER_BULB = RegisterBlockItem(g_blocks.COPPER_BULB);
  Item EXPOSED_COPPER_BULB = RegisterBlockItem(g_blocks.EXPOSED_COPPER_BULB);
  Item OXIDIZED_COPPER_BULB = RegisterBlockItem(g_blocks.OXIDIZED_COPPER_BULB);
  Item WEATHERED_COPPER_BULB =
      RegisterBlockItem(g_blocks.WEATHERED_COPPER_BULB);
  Item CRAFTING_TABLE = RegisterBlockItem(g_blocks.CRAFTING_TABLE);
  Item COPPER_GRATE = RegisterBlockItem(g_blocks.COPPER_GRATE);
  Item EXPOSED_COPPER_GRATE = RegisterBlockItem(g_blocks.EXPOSED_COPPER_GRATE);
  Item OXIDIZED_COPPER_GRATE =
      RegisterBlockItem(g_blocks.OXIDIZED_COPPER_GRATE);
  Item WEATHERED_COPPER_GRATE =
      RegisterBlockItem(g_blocks.WEATHERED_COPPER_GRATE);
  Item CUT_COPPER = RegisterBlockItem(g_blocks.CUT_COPPER);
  Item EXPOSED_CUT_COPPER = RegisterBlockItem(g_blocks.EXPOSED_CUT_COPPER);
  Item OXIDIZED_CUT_COPPER = RegisterBlockItem(g_blocks.OXIDIZED_CUT_COPPER);
  Item WEATHERED_CUT_COPPER = RegisterBlockItem(g_blocks.WEATHERED_CUT_COPPER);
  Item CHISELED_COPPER = RegisterBlockItem(g_blocks.CHISELED_COPPER);
  Item EXPOSED_CHISELED_COPPER =
      RegisterBlockItem(g_blocks.EXPOSED_CHISELED_COPPER);
  Item OXIDIZED_CHISELED_COPPER =
      RegisterBlockItem(g_blocks.OXIDIZED_CHISELED_COPPER);
  Item WEATHERED_CHISELED_COPPER =
      RegisterBlockItem(g_blocks.WEATHERED_CHISELED_COPPER);
  Item WARPED_HYPHAE = RegisterBlockItem(g_blocks.WARPED_HYPHAE);
  Item STRIPPED_WARPED_HYPHAE =
      RegisterBlockItem(g_blocks.STRIPPED_WARPED_HYPHAE);
  Item CRIMSON_HYPHAE = RegisterBlockItem(g_blocks.CRIMSON_HYPHAE);
  Item STRIPPED_CRIMSON_HYPHAE =
      RegisterBlockItem(g_blocks.STRIPPED_CRIMSON_HYPHAE);
  Item MOSSY_COBBLESTONE = RegisterBlockItem(g_blocks.MOSSY_COBBLESTONE);
  Item DEEPSLATE = RegisterBlockItem(g_blocks.DEEPSLATE);
  Item COBBLED_DEEPSLATE = RegisterBlockItem(g_blocks.COBBLED_DEEPSLATE);
  Item CHISELED_DEEPSLATE = RegisterBlockItem(g_blocks.CHISELED_DEEPSLATE);
  Item POLISHED_DEEPSLATE = RegisterBlockItem(g_blocks.POLISHED_DEEPSLATE);
  Item DEEPSLATE_TILES = RegisterBlockItem(g_blocks.DEEPSLATE_TILES);
  Item CRACKED_DEEPSLATE_TILES =
      RegisterBlockItem(g_blocks.CRACKED_DEEPSLATE_TILES);
  Item REINFORCED_DEEPSLATE = RegisterBlockItem(g_blocks.REINFORCED_DEEPSLATE);
  Item TUFF = RegisterBlockItem(g_blocks.TUFF);
  Item CHISELED_TUFF = RegisterBlockItem(g_blocks.CHISELED_TUFF);
  Item POLISHED_TUFF = RegisterBlockItem(g_blocks.POLISHED_TUFF);
  Item PACKED_MUD = RegisterBlockItem(g_blocks.PACKED_MUD);
  Item SANDSTONE = RegisterBlockItem(g_blocks.SANDSTONE);
  Item CHISELED_SANDSTONE = RegisterBlockItem(g_blocks.CHISELED_SANDSTONE);
  Item SMOOTH_SANDSTONE = RegisterBlockItem(g_blocks.SMOOTH_SANDSTONE);
  Item CUT_SANDSTONE = RegisterBlockItem(g_blocks.CUT_SANDSTONE);
  Item RED_SANDSTONE = RegisterBlockItem(g_blocks.RED_SANDSTONE);
  Item CHISELED_RED_SANDSTONE =
      RegisterBlockItem(g_blocks.CHISELED_RED_SANDSTONE);
  Item SMOOTH_RED_SANDSTONE = RegisterBlockItem(g_blocks.SMOOTH_RED_SANDSTONE);
  Item CUT_RED_SANDSTONE = RegisterBlockItem(g_blocks.CUT_RED_SANDSTONE);
  Item DARK_PRISMARINE = RegisterBlockItem(g_blocks.DARK_PRISMARINE);
  Item PURPUR_BLOCK = RegisterBlockItem(g_blocks.PURPUR_BLOCK);
  Item PURPUR_PILLAR = RegisterBlockItem(g_blocks.PURPUR_PILLAR);
  Item COAL_BLOCK = RegisterBlockItem(g_blocks.COAL_BLOCK);
  Item IRON_BLOCK = RegisterBlockItem(g_blocks.IRON_BLOCK);
  Item GOLD_BLOCK = RegisterBlockItem(g_blocks.GOLD_BLOCK);
  Item REDSTONE_BLOCK = RegisterBlockItem(g_blocks.REDSTONE_BLOCK);
  Item EMERALD_BLOCK = RegisterBlockItem(g_blocks.EMERALD_BLOCK);
  Item LAPIS_BLOCK = RegisterBlockItem(g_blocks.LAPIS_BLOCK);
  Item DIAMOND_BLOCK = RegisterBlockItem(g_blocks.DIAMOND_BLOCK);
  Item NETHERITE_BLOCK = RegisterBlockItem(g_blocks.NETHERITE_BLOCK);
  Item QUARTZ_BLOCK = RegisterBlockItem(g_blocks.QUARTZ_BLOCK);
  Item CHISELED_QUARTZ_BLOCK =
      RegisterBlockItem(g_blocks.CHISELED_QUARTZ_BLOCK);
  Item QUARTZ_PILLAR = RegisterBlockItem(g_blocks.QUARTZ_PILLAR);
  Item SMOOTH_QUARTZ = RegisterBlockItem(g_blocks.SMOOTH_QUARTZ);

  Item ANVIL = RegisterBlockItem(g_blocks.ANVIL);
  Item GetItem(ItemID id);

  ItemID GetBlockItem(BlockID block);

  int GetTotalItemCount();

  void RegisterAll();
};

extern ItemList g_items;

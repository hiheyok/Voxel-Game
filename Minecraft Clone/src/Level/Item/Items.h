// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Item/Item.h"

class GameContext;

class ItemList {
 private:
  Item RegisterBlockItem(BlockID block);

 public:
  explicit ItemList(GameContext& context);
  ~ItemList();
  Item GetItem(ItemID id);
  ItemID GetBlockItem(BlockID block);
  int GetTotalItemCount();
  void RegisterAll();

  GameContext& context_;
  int item_count_ = 0;
  FastHashMap<BlockID, ItemID> block_item_ids_;
  FastHashMap<ItemID, Item> item_container_;

  Item AIR = RegisterBlockItem(context_.blocks_->AIR);
  Item STONE = RegisterBlockItem(context_.blocks_->STONE);
  Item DIRT = RegisterBlockItem(context_.blocks_->DIRT);
  Item WATER = RegisterBlockItem(context_.blocks_->WATER);
  Item LAVA = RegisterBlockItem(context_.blocks_->LAVA);
  Item GRASS = RegisterBlockItem(context_.blocks_->GRASS);
  Item SAND = RegisterBlockItem(context_.blocks_->SAND);
  Item OAK_LOG = RegisterBlockItem(context_.blocks_->OAK_LOG);
  Item STRIPPED_OAK_LOG = RegisterBlockItem(context_.blocks_->STRIPPED_OAK_LOG);
  Item OAK_LEAF = RegisterBlockItem(context_.blocks_->OAK_LEAF);
  Item OAK_PLANKS = RegisterBlockItem(context_.blocks_->OAK_PLANKS);
  Item BAMBOO_PLANKS = RegisterBlockItem(context_.blocks_->BAMBOO_PLANKS);
  Item ACACIA_PLANKS = RegisterBlockItem(context_.blocks_->ACACIA_PLANKS);
  Item BIRCH_PLANKS = RegisterBlockItem(context_.blocks_->BIRCH_PLANKS);
  Item CHERRY_PLANKS = RegisterBlockItem(context_.blocks_->CHERRY_PLANKS);
  Item CRIMSON_PLANKS = RegisterBlockItem(context_.blocks_->CRIMSON_PLANKS);
  Item DARK_OAK_PLANKS = RegisterBlockItem(context_.blocks_->DARK_OAK_PLANKS);
  Item JUNGLE_PLANKS = RegisterBlockItem(context_.blocks_->JUNGLE_PLANKS);
  Item MANGROVE_PLANKS = RegisterBlockItem(context_.blocks_->MANGROVE_PLANKS);
  Item SPRUCE_PLANKS = RegisterBlockItem(context_.blocks_->SPRUCE_PLANKS);
  Item WARPED_PLANKS = RegisterBlockItem(context_.blocks_->WARPED_PLANKS);
  Item OAK_WOOD = RegisterBlockItem(context_.blocks_->OAK_WOOD);
  Item ACACIA_WOOD = RegisterBlockItem(context_.blocks_->ACACIA_WOOD);
  Item BIRCH_WOOD = RegisterBlockItem(context_.blocks_->BIRCH_WOOD);
  Item CHERRY_WOOD = RegisterBlockItem(context_.blocks_->CHERRY_WOOD);
  Item DARK_OAK_WOOD = RegisterBlockItem(context_.blocks_->DARK_OAK_WOOD);
  Item JUNGLE_WOOD = RegisterBlockItem(context_.blocks_->JUNGLE_WOOD);
  Item MANGROVE_WOOD = RegisterBlockItem(context_.blocks_->MANGROVE_WOOD);
  Item SPRUCE_WOOD = RegisterBlockItem(context_.blocks_->SPRUCE_WOOD);
  Item STRIPPED_OAK_WOOD =
      RegisterBlockItem(context_.blocks_->STRIPPED_OAK_WOOD);
  Item STRIPPED_ACACIA_WOOD =
      RegisterBlockItem(context_.blocks_->STRIPPED_ACACIA_WOOD);
  Item STRIPPED_BIRCH_WOOD =
      RegisterBlockItem(context_.blocks_->STRIPPED_BIRCH_WOOD);
  Item STRIPPED_CHERRY_WOOD =
      RegisterBlockItem(context_.blocks_->STRIPPED_CHERRY_WOOD);
  Item STRIPPED_DARK_OAK_WOOD =
      RegisterBlockItem(context_.blocks_->STRIPPED_DARK_OAK_WOOD);
  Item STRIPPED_JUNGLE_WOOD =
      RegisterBlockItem(context_.blocks_->STRIPPED_JUNGLE_WOOD);
  Item STRIPPED_MANGROVE_WOOD =
      RegisterBlockItem(context_.blocks_->STRIPPED_MANGROVE_WOOD);
  Item STRIPPED_SPRUCE_WOOD =
      RegisterBlockItem(context_.blocks_->STRIPPED_SPRUCE_WOOD);
  Item BRICKS = RegisterBlockItem(context_.blocks_->BRICKS);
  Item STONE_BRICKS = RegisterBlockItem(context_.blocks_->STONE_BRICKS);
  Item CRACKED_STONE_BRICKS =
      RegisterBlockItem(context_.blocks_->CRACKED_STONE_BRICKS);
  Item CHISELED_STONE_BRICKS =
      RegisterBlockItem(context_.blocks_->CHISELED_STONE_BRICKS);
  Item MOSSY_STONE_BRICKS =
      RegisterBlockItem(context_.blocks_->MOSSY_STONE_BRICKS);
  Item DEEPSLATE_BRICKS = RegisterBlockItem(context_.blocks_->DEEPSLATE_BRICKS);
  Item CRACKED_DEEPSLATE_BRICKS =
      RegisterBlockItem(context_.blocks_->CRACKED_DEEPSLATE_BRICKS);
  Item TUFF_BRICKS = RegisterBlockItem(context_.blocks_->TUFF_BRICKS);
  Item CHISELED_TUFF_BRICKS =
      RegisterBlockItem(context_.blocks_->CHISELED_TUFF_BRICKS);
  Item MUD_BRICKS = RegisterBlockItem(context_.blocks_->MUD_BRICKS);
  Item PRISMARINE_BRICKS =
      RegisterBlockItem(context_.blocks_->PRISMARINE_BRICKS);
  Item NETHER_BRICKS = RegisterBlockItem(context_.blocks_->NETHER_BRICKS);
  Item CRACKED_NETHER_BRICKS =
      RegisterBlockItem(context_.blocks_->CRACKED_NETHER_BRICKS);
  Item CHISELED_NETHER_BRICKS =
      RegisterBlockItem(context_.blocks_->CHISELED_NETHER_BRICKS);
  Item RED_NETHER_BRICKS =
      RegisterBlockItem(context_.blocks_->RED_NETHER_BRICKS);
  Item POLISHED_BLACKSTONE_BRICK =
      RegisterBlockItem(context_.blocks_->POLISHED_BLACKSTONE_BRICK);
  Item CRACKED_POLISHED_BLACKSTONE_BRICK =
      RegisterBlockItem(context_.blocks_->CRACKED_POLISHED_BLACKSTONE_BRICK);
  Item END_STONE_BRICKS = RegisterBlockItem(context_.blocks_->END_STONE_BRICKS);
  Item COBBLESTONE = RegisterBlockItem(context_.blocks_->COBBLESTONE);
  Item BLUE_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->BLUE_STAINED_GLASS);
  Item GREEN_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->GREEN_STAINED_GLASS);
  Item BLACK_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->BLACK_STAINED_GLASS);
  Item BROWN_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->BROWN_STAINED_GLASS);
  Item CYAN_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->CYAN_STAINED_GLASS);
  Item GRAY_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->GRAY_STAINED_GLASS);
  Item LIGHT_BLUE_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->LIGHT_BLUE_STAINED_GLASS);
  Item LIGHT_GRAY_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->LIGHT_GRAY_STAINED_GLASS);
  Item LIME_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->LIME_STAINED_GLASS);
  Item MAGENTA_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->MAGENTA_STAINED_GLASS);
  Item AIORANGE_STAINED_GLASSR =
      RegisterBlockItem(context_.blocks_->ORANGE_STAINED_GLASS);
  Item PINK_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->PINK_STAINED_GLASS);
  Item PURPLE_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->PURPLE_STAINED_GLASS);
  Item RED_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->RED_STAINED_GLASS);
  Item WHITE_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->WHITE_STAINED_GLASS);
  Item YELLOW_STAINED_GLASS =
      RegisterBlockItem(context_.blocks_->YELLOW_STAINED_GLASS);
  Item WHITE_WOOL = RegisterBlockItem(context_.blocks_->WHITE_WOOL);
  Item BLACK_WOOL = RegisterBlockItem(context_.blocks_->BLACK_WOOL);
  Item BLUE_WOOL = RegisterBlockItem(context_.blocks_->BLUE_WOOL);
  Item BROWN_WOOL = RegisterBlockItem(context_.blocks_->BROWN_WOOL);
  Item CYAN_WOOL = RegisterBlockItem(context_.blocks_->CYAN_WOOL);
  Item GRAY_WOOL = RegisterBlockItem(context_.blocks_->GRAY_WOOL);
  Item GREEN_WOOL = RegisterBlockItem(context_.blocks_->GREEN_WOOL);
  Item LIGHT_BLUE_WOOL = RegisterBlockItem(context_.blocks_->LIGHT_BLUE_WOOL);
  Item LIGHT_GRAY_WOOL = RegisterBlockItem(context_.blocks_->LIGHT_GRAY_WOOL);
  Item LIME_WOOL = RegisterBlockItem(context_.blocks_->LIME_WOOL);
  Item MAGENTA_WOOL = RegisterBlockItem(context_.blocks_->MAGENTA_WOOL);
  Item ORANGE_WOOL = RegisterBlockItem(context_.blocks_->ORANGE_WOOL);
  Item PINK_WOOL = RegisterBlockItem(context_.blocks_->PINK_WOOL);
  Item PURPLE_WOOL = RegisterBlockItem(context_.blocks_->PURPLE_WOOL);
  Item RED_WOOL = RegisterBlockItem(context_.blocks_->RED_WOOL);
  Item YELLOW_WOOL = RegisterBlockItem(context_.blocks_->YELLOW_WOOL);
  Item WHITE_TERRACOTTA = RegisterBlockItem(context_.blocks_->WHITE_TERRACOTTA);
  Item BLACK_TERRACOTTA = RegisterBlockItem(context_.blocks_->BLACK_TERRACOTTA);
  Item BLUE_TERRACOTTA = RegisterBlockItem(context_.blocks_->BLUE_TERRACOTTA);
  Item BROWN_TERRACOTTA = RegisterBlockItem(context_.blocks_->BROWN_TERRACOTTA);
  Item CYAN_TERRACOTTA = RegisterBlockItem(context_.blocks_->CYAN_TERRACOTTA);
  Item GRAY_TERRACOTTA = RegisterBlockItem(context_.blocks_->GRAY_TERRACOTTA);
  Item GREEN_TERRACOTTA = RegisterBlockItem(context_.blocks_->GREEN_TERRACOTTA);
  Item LIGHT_BLUE_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->LIGHT_BLUE_TERRACOTTA);
  Item LIGHT_GRAY_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->LIGHT_GRAY_TERRACOTTA);
  Item LIME_TERRACOTTA = RegisterBlockItem(context_.blocks_->LIME_TERRACOTTA);
  Item MAGENTA_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->MAGENTA_TERRACOTTA);
  Item ORANGE_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->ORANGE_TERRACOTTA);
  Item PINK_TERRACOTTA = RegisterBlockItem(context_.blocks_->PINK_TERRACOTTA);
  Item PURPLE_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->PURPLE_TERRACOTTA);
  Item RED_TERRACOTTA = RegisterBlockItem(context_.blocks_->RED_TERRACOTTA);
  Item YELLOW_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->YELLOW_TERRACOTTA);
  Item WHITE_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->WHITE_GLAZED_TERRACOTTA);
  Item BLACK_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->BLACK_GLAZED_TERRACOTTA);
  Item BLUE_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->BLUE_GLAZED_TERRACOTTA);
  Item BROWN_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->BROWN_GLAZED_TERRACOTTA);
  Item CYAN_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->CYAN_GLAZED_TERRACOTTA);
  Item GRAY_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->GRAY_GLAZED_TERRACOTTA);
  Item GREEN_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->GREEN_GLAZED_TERRACOTTA);
  Item LIGHT_BLUE_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->LIGHT_BLUE_GLAZED_TERRACOTTA);
  Item LIGHT_GRAY_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->LIGHT_GRAY_GLAZED_TERRACOTTA);
  Item LIME_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->LIME_GLAZED_TERRACOTTA);
  Item MAGENTA_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->MAGENTA_GLAZED_TERRACOTTA);
  Item ORANGE_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->ORANGE_GLAZED_TERRACOTTA);
  Item PINK_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->PINK_GLAZED_TERRACOTTA);
  Item PURPLE_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->PURPLE_GLAZED_TERRACOTTA);
  Item RED_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->RED_GLAZED_TERRACOTTA);
  Item YELLOW_GLAZED_TERRACOTTA =
      RegisterBlockItem(context_.blocks_->YELLOW_GLAZED_TERRACOTTA);
  Item TERRACOTTA = RegisterBlockItem(context_.blocks_->TERRACOTTA);
  Item WHITE_CONCRETE = RegisterBlockItem(context_.blocks_->WHITE_CONCRETE);
  Item BLACK_CONCRETE = RegisterBlockItem(context_.blocks_->BLACK_CONCRETE);
  Item BLUE_CONCRETE = RegisterBlockItem(context_.blocks_->BLUE_CONCRETE);
  Item BROWN_CONCRETE = RegisterBlockItem(context_.blocks_->BROWN_CONCRETE);
  Item CYAN_CONCRETE = RegisterBlockItem(context_.blocks_->CYAN_CONCRETE);
  Item GRAY_CONCRETE = RegisterBlockItem(context_.blocks_->GRAY_CONCRETE);
  Item GREEN_CONCRETE = RegisterBlockItem(context_.blocks_->GREEN_CONCRETE);
  Item LIGHT_BLUE_CONCRETE =
      RegisterBlockItem(context_.blocks_->LIGHT_BLUE_CONCRETE);
  Item LIGHT_GRAY_CONCRETE =
      RegisterBlockItem(context_.blocks_->LIGHT_GRAY_CONCRETE);
  Item LIME_CONCRETE = RegisterBlockItem(context_.blocks_->LIME_CONCRETE);
  Item MAGENTA_CONCRETE = RegisterBlockItem(context_.blocks_->MAGENTA_CONCRETE);
  Item ORANGE_CONCRETE = RegisterBlockItem(context_.blocks_->ORANGE_CONCRETE);
  Item PINK_CONCRETE = RegisterBlockItem(context_.blocks_->PINK_CONCRETE);
  Item PURPLE_CONCRETE = RegisterBlockItem(context_.blocks_->PURPLE_CONCRETE);
  Item RED_CONCRETE = RegisterBlockItem(context_.blocks_->RED_CONCRETE);
  Item YELLOW_CONCRETE = RegisterBlockItem(context_.blocks_->YELLOW_CONCRETE);
  Item WHITE_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->WHITE_CONCRETE_POWDER);
  Item BLACK_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->BLACK_CONCRETE_POWDER);
  Item BLUE_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->BLUE_CONCRETE_POWDER);
  Item BROWN_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->BROWN_CONCRETE_POWDER);
  Item CYAN_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->CYAN_CONCRETE_POWDER);
  Item GRAY_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->GRAY_CONCRETE_POWDER);
  Item GREEN_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->GREEN_CONCRETE_POWDER);
  Item LIGHT_BLUE_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->LIGHT_BLUE_CONCRETE_POWDER);
  Item LIGHT_GRAY_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->LIGHT_GRAY_CONCRETE_POWDER);
  Item LIME_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->LIME_CONCRETE_POWDER);
  Item MAGENTA_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->MAGENTA_CONCRETE_POWDER);
  Item ORANGE_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->ORANGE_CONCRETE_POWDER);
  Item PINK_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->PINK_CONCRETE_POWDER);
  Item PURPLE_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->PURPLE_CONCRETE_POWDER);
  Item RED_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->RED_CONCRETE_POWDER);
  Item YELLOW_CONCRETE_POWDER =
      RegisterBlockItem(context_.blocks_->YELLOW_CONCRETE_POWDER);
  Item PODZOL = RegisterBlockItem(context_.blocks_->PODZOL);
  Item MYCELIUM = RegisterBlockItem(context_.blocks_->MYCELIUM);
  Item DIRT_PATH = RegisterBlockItem(context_.blocks_->DIRT_PATH);
  Item COARSE_DIRT = RegisterBlockItem(context_.blocks_->COARSE_DIRT);
  Item ROOTED_DIRT = RegisterBlockItem(context_.blocks_->ROOTED_DIRT);
  Item FARMLAND = RegisterBlockItem(context_.blocks_->FARMLAND);
  Item FARMLAND_MOIST = RegisterBlockItem(context_.blocks_->FARMLAND_MOIST);
  Item MUD = RegisterBlockItem(context_.blocks_->MUD);
  Item CLAY = RegisterBlockItem(context_.blocks_->CLAY);
  Item GRAVEL = RegisterBlockItem(context_.blocks_->GRAVEL);
  Item RED_SAND = RegisterBlockItem(context_.blocks_->RED_SAND);
  Item ICE = RegisterBlockItem(context_.blocks_->ICE);
  Item BLUE_ICE = RegisterBlockItem(context_.blocks_->BLUE_ICE);
  Item SNOW_BLOCK = RegisterBlockItem(context_.blocks_->SNOW_BLOCK);
  Item MOSS_BLOCK = RegisterBlockItem(context_.blocks_->MOSS_BLOCK);
  Item GRANITE = RegisterBlockItem(context_.blocks_->GRANITE);
  Item DIORITE = RegisterBlockItem(context_.blocks_->DIORITE);
  Item ANDESITE = RegisterBlockItem(context_.blocks_->ANDESITE);
  Item CALCITE = RegisterBlockItem(context_.blocks_->CALCITE);
  Item DRIPSTONE_BLOCK = RegisterBlockItem(context_.blocks_->DRIPSTONE_BLOCK);
  Item PRISMARINE = RegisterBlockItem(context_.blocks_->PRISMARINE);
  Item MAGMA = RegisterBlockItem(context_.blocks_->MAGMA);
  Item OBSIDIAN = RegisterBlockItem(context_.blocks_->OBSIDIAN);
  Item CRYING_OBSIDIAN = RegisterBlockItem(context_.blocks_->CRYING_OBSIDIAN);
  Item NETHERRACK = RegisterBlockItem(context_.blocks_->NETHERRACK);
  Item CRIMSON_NYLIUM = RegisterBlockItem(context_.blocks_->CRIMSON_NYLIUM);
  Item WARPED_NYLIUM = RegisterBlockItem(context_.blocks_->WARPED_NYLIUM);
  Item SOUL_SAND = RegisterBlockItem(context_.blocks_->SOUL_SAND);
  Item SOUL_SOIL = RegisterBlockItem(context_.blocks_->SOUL_SOIL);
  Item BONE_BLOCK = RegisterBlockItem(context_.blocks_->BONE_BLOCK);
  Item BLACKSTONE = RegisterBlockItem(context_.blocks_->BLACKSTONE);
  Item BASALT = RegisterBlockItem(context_.blocks_->BASALT);
  Item SMOOTH_BASALT = RegisterBlockItem(context_.blocks_->SMOOTH_BASALT);
  Item END_STONE = RegisterBlockItem(context_.blocks_->END_STONE);
  Item COAL_ORE = RegisterBlockItem(context_.blocks_->COAL_ORE);
  Item DEEPSLATE_COAL_ORE =
      RegisterBlockItem(context_.blocks_->DEEPSLATE_COAL_ORE);
  Item IRON_ORE = RegisterBlockItem(context_.blocks_->IRON_ORE);
  Item DEEPSLATE_IRON_ORE =
      RegisterBlockItem(context_.blocks_->DEEPSLATE_IRON_ORE);
  Item COPPER_ORE = RegisterBlockItem(context_.blocks_->COPPER_ORE);
  Item DEEPSLATE_COPPER_ORE =
      RegisterBlockItem(context_.blocks_->DEEPSLATE_COPPER_ORE);
  Item GOLD_ORE = RegisterBlockItem(context_.blocks_->GOLD_ORE);
  Item DEEPSLATE_GOLD_ORE =
      RegisterBlockItem(context_.blocks_->DEEPSLATE_GOLD_ORE);
  Item REDSTONE_ORE = RegisterBlockItem(context_.blocks_->REDSTONE_ORE);
  Item DEEPSLATE_REDSTONE_ORE =
      RegisterBlockItem(context_.blocks_->DEEPSLATE_REDSTONE_ORE);
  Item EMERALD_ORE = RegisterBlockItem(context_.blocks_->EMERALD_ORE);
  Item DEEPSLATE_EMERALD_ORE =
      RegisterBlockItem(context_.blocks_->DEEPSLATE_EMERALD_ORE);
  Item LAPIS_ORE = RegisterBlockItem(context_.blocks_->LAPIS_ORE);
  Item DEEPSLATE_LAPIS_ORE =
      RegisterBlockItem(context_.blocks_->DEEPSLATE_LAPIS_ORE);
  Item DIAMOND_ORE = RegisterBlockItem(context_.blocks_->DIAMOND_ORE);
  Item DEEPSLATE_DIAMOND_ORE =
      RegisterBlockItem(context_.blocks_->DEEPSLATE_DIAMOND_ORE);
  Item NETHER_GOLD_ORE = RegisterBlockItem(context_.blocks_->NETHER_GOLD_ORE);
  Item NETHER_QUARTZ_ORE =
      RegisterBlockItem(context_.blocks_->NETHER_QUARTZ_ORE);
  Item ANCIENT_DEBRIS = RegisterBlockItem(context_.blocks_->ANCIENT_DEBRIS);
  Item RAW_IRON_BLOCK = RegisterBlockItem(context_.blocks_->RAW_IRON_BLOCK);
  Item RAW_COPPER_BLOCK = RegisterBlockItem(context_.blocks_->RAW_COPPER_BLOCK);
  Item RAW_GOLD_BLOCK = RegisterBlockItem(context_.blocks_->RAW_GOLD_BLOCK);
  Item GLOWSTONE = RegisterBlockItem(context_.blocks_->GLOWSTONE);
  Item AMETHYST_BLOCK = RegisterBlockItem(context_.blocks_->AMETHYST_BLOCK);
  Item BUDDING_AMETHYST = RegisterBlockItem(context_.blocks_->BUDDING_AMETHYST);
  Item SPRUCE_LOG = RegisterBlockItem(context_.blocks_->SPRUCE_LOG);
  Item STRIPPED_SPRUCE_LOG =
      RegisterBlockItem(context_.blocks_->STRIPPED_SPRUCE_LOG);
  Item BIRCH_LOG = RegisterBlockItem(context_.blocks_->BIRCH_LOG);
  Item STRIPPED_BIRCH_LOG =
      RegisterBlockItem(context_.blocks_->STRIPPED_BIRCH_LOG);
  Item JUNGLE_LOG = RegisterBlockItem(context_.blocks_->JUNGLE_LOG);
  Item STRIPPED_JUNGLE_LOG =
      RegisterBlockItem(context_.blocks_->STRIPPED_JUNGLE_LOG);
  Item ACACIA_LOG = RegisterBlockItem(context_.blocks_->ACACIA_LOG);
  Item STRIPPED_ACACIA_LOG =
      RegisterBlockItem(context_.blocks_->STRIPPED_ACACIA_LOG);
  Item DARK_OAK_LOG = RegisterBlockItem(context_.blocks_->DARK_OAK_LOG);
  Item STRIPPED_DARK_OAK_LOG =
      RegisterBlockItem(context_.blocks_->STRIPPED_DARK_OAK_LOG);
  Item CHERRY_LOG = RegisterBlockItem(context_.blocks_->CHERRY_LOG);
  Item STRIPPED_CHERRY_LOG =
      RegisterBlockItem(context_.blocks_->STRIPPED_CHERRY_LOG);
  Item MANGROVE_LOG = RegisterBlockItem(context_.blocks_->MANGROVE_LOG);
  Item STRIPPED_MANGROVE_LOG =
      RegisterBlockItem(context_.blocks_->STRIPPED_MANGROVE_LOG);
  Item CRIMSON_STEM = RegisterBlockItem(context_.blocks_->CRIMSON_STEM);
  Item STRIPPED_CRIMSON_STEM =
      RegisterBlockItem(context_.blocks_->STRIPPED_CRIMSON_STEM);
  Item WARPED_STEM = RegisterBlockItem(context_.blocks_->WARPED_STEM);
  Item STRIPPED_WARPED_STEM =
      RegisterBlockItem(context_.blocks_->STRIPPED_WARPED_STEM);
  Item MANGROVE_ROOTS = RegisterBlockItem(context_.blocks_->MANGROVE_ROOTS);
  Item MUDDY_MANGROVE_ROOTS =
      RegisterBlockItem(context_.blocks_->MUDDY_MANGROVE_ROOTS);
  Item MUSHROOM_STEM = RegisterBlockItem(context_.blocks_->MUSHROOM_STEM);
  Item SPRUCE_LEAVES = RegisterBlockItem(context_.blocks_->SPRUCE_LEAVES);
  Item BIRCH_LEAVES = RegisterBlockItem(context_.blocks_->BIRCH_LEAVES);
  Item JUNGLE_LEAVES = RegisterBlockItem(context_.blocks_->JUNGLE_LEAVES);
  Item ACACIA_LEAVES = RegisterBlockItem(context_.blocks_->ACACIA_LEAVES);
  Item DARK_OAK_LEAVES = RegisterBlockItem(context_.blocks_->DARK_OAK_LEAVES);
  Item MANGROVE_LEAVES = RegisterBlockItem(context_.blocks_->MANGROVE_LEAVES);
  Item CHERRY_LEAVES = RegisterBlockItem(context_.blocks_->CHERRY_LEAVES);
  Item AZALEA_LEAVES = RegisterBlockItem(context_.blocks_->AZALEA_LEAVES);
  Item FLOWERING_AZALEA_LEAVES =
      RegisterBlockItem(context_.blocks_->FLOWERING_AZALEA_LEAVES);
  Item BROWN_MUSHROOM_BLOCK =
      RegisterBlockItem(context_.blocks_->BROWN_MUSHROOM_BLOCK);
  Item RED_MUSHROOM_BLOCK =
      RegisterBlockItem(context_.blocks_->RED_MUSHROOM_BLOCK);
  Item NETHER_WART_BLOCK =
      RegisterBlockItem(context_.blocks_->NETHER_WART_BLOCK);
  Item WARPED_WART_BLOCK =
      RegisterBlockItem(context_.blocks_->WARPED_WART_BLOCK);
  Item SHROOMLIGHT = RegisterBlockItem(context_.blocks_->SHROOMLIGHT);
  Item DRIED_KELP_BLOCK = RegisterBlockItem(context_.blocks_->DRIED_KELP_BLOCK);
  Item TUBE_CORAL_BLOCK = RegisterBlockItem(context_.blocks_->TUBE_CORAL_BLOCK);
  Item BRAIN_CORAL_BLOCK =
      RegisterBlockItem(context_.blocks_->BRAIN_CORAL_BLOCK);
  Item BUBBLE_CORAL_BLOCK =
      RegisterBlockItem(context_.blocks_->BUBBLE_CORAL_BLOCK);
  Item FIRE_CORAL_BLOCK = RegisterBlockItem(context_.blocks_->FIRE_CORAL_BLOCK);
  Item HORN_CORAL_BLOCK = RegisterBlockItem(context_.blocks_->HORN_CORAL_BLOCK);
  Item DEAD_TUBE_CORAL_BLOCK =
      RegisterBlockItem(context_.blocks_->DEAD_TUBE_CORAL_BLOCK);
  Item DEAD_BRAIN_CORAL_BLOCK =
      RegisterBlockItem(context_.blocks_->DEAD_BRAIN_CORAL_BLOCK);
  Item DEAD_BUBBLE_CORAL_BLOCK =
      RegisterBlockItem(context_.blocks_->DEAD_BUBBLE_CORAL_BLOCK);
  Item DEAD_FIRE_CORAL_BLOCK =
      RegisterBlockItem(context_.blocks_->DEAD_FIRE_CORAL_BLOCK);
  Item DEAD_HORN_CORAL_BLOCK =
      RegisterBlockItem(context_.blocks_->DEAD_HORN_CORAL_BLOCK);
  Item SPONGE = RegisterBlockItem(context_.blocks_->SPONGE);
  Item WET_SPONGE = RegisterBlockItem(context_.blocks_->WET_SPONGE);
  Item MELON = RegisterBlockItem(context_.blocks_->MELON);
  Item PUMPKIN = RegisterBlockItem(context_.blocks_->PUMPKIN);
  Item CARVED_PUMPKIN = RegisterBlockItem(context_.blocks_->CARVED_PUMPKIN);
  Item JACK_O_LANTERN = RegisterBlockItem(context_.blocks_->JACK_O_LANTERN);
  Item HAY_BALE = RegisterBlockItem(context_.blocks_->HAY_BALE);
  Item BEE_NEST = RegisterBlockItem(context_.blocks_->BEE_NEST);
  Item HONEYCOMB_BLOCK = RegisterBlockItem(context_.blocks_->HONEYCOMB_BLOCK);
  Item OCHRE_FROGLIGHT = RegisterBlockItem(context_.blocks_->OCHRE_FROGLIGHT);
  Item VERDANT_FROGLIGHT =
      RegisterBlockItem(context_.blocks_->VERDANT_FROGLIGHT);
  Item PEARLESCENT_FROGLIGHT =
      RegisterBlockItem(context_.blocks_->PEARLESCENT_FROGLIGHT);
  Item SCULK = RegisterBlockItem(context_.blocks_->SCULK);
  Item SCULK_CATALYST = RegisterBlockItem(context_.blocks_->SCULK_CATALYST);
  Item BEDROCK = RegisterBlockItem(context_.blocks_->BEDROCK);
  Item SEA_LANTERN = RegisterBlockItem(context_.blocks_->SEA_LANTERN);
  Item REDSTONE_LAMP = RegisterBlockItem(context_.blocks_->REDSTONE_LAMP);
  Item COPPER_BULB = RegisterBlockItem(context_.blocks_->COPPER_BULB);
  Item EXPOSED_COPPER_BULB =
      RegisterBlockItem(context_.blocks_->EXPOSED_COPPER_BULB);
  Item OXIDIZED_COPPER_BULB =
      RegisterBlockItem(context_.blocks_->OXIDIZED_COPPER_BULB);
  Item WEATHERED_COPPER_BULB =
      RegisterBlockItem(context_.blocks_->WEATHERED_COPPER_BULB);
  Item CRAFTING_TABLE = RegisterBlockItem(context_.blocks_->CRAFTING_TABLE);
  Item COPPER_GRATE = RegisterBlockItem(context_.blocks_->COPPER_GRATE);
  Item EXPOSED_COPPER_GRATE =
      RegisterBlockItem(context_.blocks_->EXPOSED_COPPER_GRATE);
  Item OXIDIZED_COPPER_GRATE =
      RegisterBlockItem(context_.blocks_->OXIDIZED_COPPER_GRATE);
  Item WEATHERED_COPPER_GRATE =
      RegisterBlockItem(context_.blocks_->WEATHERED_COPPER_GRATE);
  Item CUT_COPPER = RegisterBlockItem(context_.blocks_->CUT_COPPER);
  Item EXPOSED_CUT_COPPER =
      RegisterBlockItem(context_.blocks_->EXPOSED_CUT_COPPER);
  Item OXIDIZED_CUT_COPPER =
      RegisterBlockItem(context_.blocks_->OXIDIZED_CUT_COPPER);
  Item WEATHERED_CUT_COPPER =
      RegisterBlockItem(context_.blocks_->WEATHERED_CUT_COPPER);
  Item CHISELED_COPPER = RegisterBlockItem(context_.blocks_->CHISELED_COPPER);
  Item EXPOSED_CHISELED_COPPER =
      RegisterBlockItem(context_.blocks_->EXPOSED_CHISELED_COPPER);
  Item OXIDIZED_CHISELED_COPPER =
      RegisterBlockItem(context_.blocks_->OXIDIZED_CHISELED_COPPER);
  Item WEATHERED_CHISELED_COPPER =
      RegisterBlockItem(context_.blocks_->WEATHERED_CHISELED_COPPER);
  Item WARPED_HYPHAE = RegisterBlockItem(context_.blocks_->WARPED_HYPHAE);
  Item STRIPPED_WARPED_HYPHAE =
      RegisterBlockItem(context_.blocks_->STRIPPED_WARPED_HYPHAE);
  Item CRIMSON_HYPHAE = RegisterBlockItem(context_.blocks_->CRIMSON_HYPHAE);
  Item STRIPPED_CRIMSON_HYPHAE =
      RegisterBlockItem(context_.blocks_->STRIPPED_CRIMSON_HYPHAE);
  Item MOSSY_COBBLESTONE =
      RegisterBlockItem(context_.blocks_->MOSSY_COBBLESTONE);
  Item DEEPSLATE = RegisterBlockItem(context_.blocks_->DEEPSLATE);
  Item COBBLED_DEEPSLATE =
      RegisterBlockItem(context_.blocks_->COBBLED_DEEPSLATE);
  Item CHISELED_DEEPSLATE =
      RegisterBlockItem(context_.blocks_->CHISELED_DEEPSLATE);
  Item POLISHED_DEEPSLATE =
      RegisterBlockItem(context_.blocks_->POLISHED_DEEPSLATE);
  Item DEEPSLATE_TILES = RegisterBlockItem(context_.blocks_->DEEPSLATE_TILES);
  Item CRACKED_DEEPSLATE_TILES =
      RegisterBlockItem(context_.blocks_->CRACKED_DEEPSLATE_TILES);
  Item REINFORCED_DEEPSLATE =
      RegisterBlockItem(context_.blocks_->REINFORCED_DEEPSLATE);
  Item TUFF = RegisterBlockItem(context_.blocks_->TUFF);
  Item CHISELED_TUFF = RegisterBlockItem(context_.blocks_->CHISELED_TUFF);
  Item POLISHED_TUFF = RegisterBlockItem(context_.blocks_->POLISHED_TUFF);
  Item PACKED_MUD = RegisterBlockItem(context_.blocks_->PACKED_MUD);
  Item SANDSTONE = RegisterBlockItem(context_.blocks_->SANDSTONE);
  Item CHISELED_SANDSTONE =
      RegisterBlockItem(context_.blocks_->CHISELED_SANDSTONE);
  Item SMOOTH_SANDSTONE = RegisterBlockItem(context_.blocks_->SMOOTH_SANDSTONE);
  Item CUT_SANDSTONE = RegisterBlockItem(context_.blocks_->CUT_SANDSTONE);
  Item RED_SANDSTONE = RegisterBlockItem(context_.blocks_->RED_SANDSTONE);
  Item CHISELED_RED_SANDSTONE =
      RegisterBlockItem(context_.blocks_->CHISELED_RED_SANDSTONE);
  Item SMOOTH_RED_SANDSTONE =
      RegisterBlockItem(context_.blocks_->SMOOTH_RED_SANDSTONE);
  Item CUT_RED_SANDSTONE =
      RegisterBlockItem(context_.blocks_->CUT_RED_SANDSTONE);
  Item DARK_PRISMARINE = RegisterBlockItem(context_.blocks_->DARK_PRISMARINE);
  Item PURPUR_BLOCK = RegisterBlockItem(context_.blocks_->PURPUR_BLOCK);
  Item PURPUR_PILLAR = RegisterBlockItem(context_.blocks_->PURPUR_PILLAR);
  Item COAL_BLOCK = RegisterBlockItem(context_.blocks_->COAL_BLOCK);
  Item IRON_BLOCK = RegisterBlockItem(context_.blocks_->IRON_BLOCK);
  Item GOLD_BLOCK = RegisterBlockItem(context_.blocks_->GOLD_BLOCK);
  Item REDSTONE_BLOCK = RegisterBlockItem(context_.blocks_->REDSTONE_BLOCK);
  Item EMERALD_BLOCK = RegisterBlockItem(context_.blocks_->EMERALD_BLOCK);
  Item LAPIS_BLOCK = RegisterBlockItem(context_.blocks_->LAPIS_BLOCK);
  Item DIAMOND_BLOCK = RegisterBlockItem(context_.blocks_->DIAMOND_BLOCK);
  Item NETHERITE_BLOCK = RegisterBlockItem(context_.blocks_->NETHERITE_BLOCK);
  Item QUARTZ_BLOCK = RegisterBlockItem(context_.blocks_->QUARTZ_BLOCK);
  Item CHISELED_QUARTZ_BLOCK =
      RegisterBlockItem(context_.blocks_->CHISELED_QUARTZ_BLOCK);
  Item QUARTZ_PILLAR = RegisterBlockItem(context_.blocks_->QUARTZ_PILLAR);
  Item SMOOTH_QUARTZ = RegisterBlockItem(context_.blocks_->SMOOTH_QUARTZ);

  Item ANVIL = RegisterBlockItem(context_.blocks_->ANVIL);
};

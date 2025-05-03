// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>
#include <vector>

#include "Core/Typenames.h"
#include "RenderEngine/BlockModel/ModelLoader.h"
#include "Type/BlockTypes.h"
class BlockTextureAtlas;

struct BlockModel;
struct BlockProperties;

class BlockList {
 public:
  FastHashMap<std::string, BlockID> block_id_name_data_;
  ModelLoader model_loader_;
  std::vector<Block*> block_type_data_;
  std::vector<BlockProperties> block_properties_;

  std::unique_ptr<BlockTextureAtlas> block_texture_atlas_;

  BlockID AIR = RegisterBlock("minecraft:air", new AirBlock());
  BlockID NULL_BLOCK = RegisterBlock("minecraft:null", new DefaultBlock());
  BlockID STONE = RegisterBlock("minecraft:stone", new DefaultBlock());
  BlockID DIRT = RegisterBlock("minecraft:dirt", new DirtBlock());
  BlockID WATER = RegisterBlock("minecraft:water", new Fluid(5));
  BlockID LAVA = RegisterBlock("minecraft:lava", new Fluid(5));
  BlockID GRASS =
      RegisterBlock("minecraft:grass_block", new GrassBlock(0.1, 0.1));
  BlockID SAND = RegisterBlock("minecraft:sand", new GravityBlock());
  BlockID OAK_LOG = RegisterBlock("minecraft:oak_log", new DefaultBlock());
  BlockID STRIPPED_OAK_LOG =
      RegisterBlock("minecraft:stripped_oak_log", new DefaultBlock());
  BlockID OAK_PLANKS =
      RegisterBlock("minecraft:oak_planks", new DefaultBlock());
  BlockID ACACIA_PLANKS =
      RegisterBlock("minecraft:acacia_planks", new DefaultBlock());
  BlockID BAMBOO_PLANKS =
      RegisterBlock("minecraft:bamboo_planks", new DefaultBlock());
  BlockID BIRCH_PLANKS =
      RegisterBlock("minecraft:birch_planks", new DefaultBlock());
  BlockID CHERRY_PLANKS =
      RegisterBlock("minecraft:cherry_planks", new DefaultBlock());
  BlockID CRIMSON_PLANKS =
      RegisterBlock("minecraft:crimson_planks", new DefaultBlock());
  BlockID DARK_OAK_PLANKS =
      RegisterBlock("minecraft:dark_oak_planks", new DefaultBlock());
  BlockID JUNGLE_PLANKS =
      RegisterBlock("minecraft:jungle_planks", new DefaultBlock());
  BlockID MANGROVE_PLANKS =
      RegisterBlock("minecraft:mangrove_planks", new DefaultBlock());
  BlockID SPRUCE_PLANKS =
      RegisterBlock("minecraft:spruce_planks", new DefaultBlock());
  BlockID WARPED_PLANKS =
      RegisterBlock("minecraft:warped_planks", new DefaultBlock());
  BlockID OAK_WOOD = RegisterBlock("minecraft:oak_wood", new DefaultBlock());
  BlockID ACACIA_WOOD =
      RegisterBlock("minecraft:acacia_wood", new DefaultBlock());
  BlockID BIRCH_WOOD =
      RegisterBlock("minecraft:birch_wood", new DefaultBlock());
  BlockID CHERRY_WOOD =
      RegisterBlock("minecraft:cherry_wood", new DefaultBlock());
  BlockID DARK_OAK_WOOD =
      RegisterBlock("minecraft:dark_oak_wood", new DefaultBlock());
  BlockID JUNGLE_WOOD =
      RegisterBlock("minecraft:jungle_wood", new DefaultBlock());
  BlockID MANGROVE_WOOD =
      RegisterBlock("minecraft:mangrove_wood", new DefaultBlock());
  BlockID SPRUCE_WOOD =
      RegisterBlock("minecraft:spruce_wood", new DefaultBlock());
  BlockID STRIPPED_OAK_WOOD =
      RegisterBlock("minecraft:stripped_oak_wood", new DefaultBlock());
  BlockID STRIPPED_ACACIA_WOOD =
      RegisterBlock("minecraft:stripped_acacia_wood", new DefaultBlock());
  BlockID STRIPPED_BIRCH_WOOD =
      RegisterBlock("minecraft:stripped_birch_wood", new DefaultBlock());
  BlockID STRIPPED_CHERRY_WOOD =
      RegisterBlock("minecraft:stripped_cherry_wood", new DefaultBlock());
  BlockID STRIPPED_DARK_OAK_WOOD =
      RegisterBlock("minecraft:stripped_dark_oak_wood", new DefaultBlock());
  BlockID STRIPPED_JUNGLE_WOOD =
      RegisterBlock("minecraft:stripped_jungle_wood", new DefaultBlock());
  BlockID STRIPPED_MANGROVE_WOOD =
      RegisterBlock("minecraft:stripped_mangrove_wood", new DefaultBlock());
  BlockID STRIPPED_SPRUCE_WOOD =
      RegisterBlock("minecraft:stripped_spruce_wood", new DefaultBlock());
  BlockID BRICKS = RegisterBlock("minecraft:bricks", new DefaultBlock());
  BlockID STONE_BRICKS =
      RegisterBlock("minecraft:stone_bricks", new DefaultBlock());
  BlockID CRACKED_STONE_BRICKS =
      RegisterBlock("minecraft:cracked_stone_bricks", new DefaultBlock());
  BlockID CHISELED_STONE_BRICKS =
      RegisterBlock("minecraft:chiseled_stone_bricks", new DefaultBlock());
  BlockID MOSSY_STONE_BRICKS =
      RegisterBlock("minecraft:mossy_stone_bricks", new DefaultBlock());
  BlockID DEEPSLATE_BRICKS =
      RegisterBlock("minecraft:deepslate_bricks", new DefaultBlock());
  BlockID CRACKED_DEEPSLATE_BRICKS =
      RegisterBlock("minecraft:cracked_deepslate_bricks", new DefaultBlock());
  BlockID TUFF_BRICKS =
      RegisterBlock("minecraft:tuff_bricks", new DefaultBlock());
  BlockID CHISELED_TUFF_BRICKS =
      RegisterBlock("minecraft:chiseled_tuff_bricks", new DefaultBlock());
  BlockID MUD_BRICKS =
      RegisterBlock("minecraft:mud_bricks", new DefaultBlock());
  BlockID PRISMARINE_BRICKS =
      RegisterBlock("minecraft:prismarine_bricks", new DefaultBlock());
  BlockID NETHER_BRICKS =
      RegisterBlock("minecraft:nether_bricks", new DefaultBlock());
  BlockID CRACKED_NETHER_BRICKS =
      RegisterBlock("minecraft:cracked_nether_bricks", new DefaultBlock());
  BlockID CHISELED_NETHER_BRICKS =
      RegisterBlock("minecraft:chiseled_nether_bricks", new DefaultBlock());
  BlockID RED_NETHER_BRICKS =
      RegisterBlock("minecraft:red_nether_bricks", new DefaultBlock());
  BlockID POLISHED_BLACKSTONE_BRICK =
      RegisterBlock("minecraft:polished_blackstone_bricks", new DefaultBlock());
  BlockID CRACKED_POLISHED_BLACKSTONE_BRICK = RegisterBlock(
      "minecraft:cracked_polished_blackstone_bricks", new DefaultBlock());
  BlockID END_STONE_BRICKS =
      RegisterBlock("minecraft:end_stone_bricks", new DefaultBlock());
  BlockID COBBLESTONE =
      RegisterBlock("minecraft:cobblestone", new DefaultBlock());
  BlockID BLUE_STAINED_GLASS =
      RegisterBlock("minecraft:blue_stained_glass", new DefaultBlock());
  BlockID GREEN_STAINED_GLASS =
      RegisterBlock("minecraft:green_stained_glass", new DefaultBlock());
  BlockID BLACK_STAINED_GLASS =
      RegisterBlock("minecraft:black_stained_glass", new DefaultBlock());
  BlockID BROWN_STAINED_GLASS =
      RegisterBlock("minecraft:brown_stained_glass", new DefaultBlock());
  BlockID CYAN_STAINED_GLASS =
      RegisterBlock("minecraft:cyan_stained_glass", new DefaultBlock());
  BlockID GRAY_STAINED_GLASS =
      RegisterBlock("minecraft:gray_stained_glass", new DefaultBlock());
  BlockID LIGHT_BLUE_STAINED_GLASS =
      RegisterBlock("minecraft:light_blue_stained_glass", new DefaultBlock());
  BlockID LIGHT_GRAY_STAINED_GLASS =
      RegisterBlock("minecraft:light_gray_stained_glass", new DefaultBlock());
  BlockID LIME_STAINED_GLASS =
      RegisterBlock("minecraft:lime_stained_glass", new DefaultBlock());
  BlockID MAGENTA_STAINED_GLASS =
      RegisterBlock("minecraft:magenta_stained_glass", new DefaultBlock());
  BlockID ORANGE_STAINED_GLASS =
      RegisterBlock("minecraft:orange_stained_glass", new DefaultBlock());
  BlockID PINK_STAINED_GLASS =
      RegisterBlock("minecraft:pink_stained_glass", new DefaultBlock());
  BlockID PURPLE_STAINED_GLASS =
      RegisterBlock("minecraft:purple_stained_glass", new DefaultBlock());
  BlockID RED_STAINED_GLASS =
      RegisterBlock("minecraft:red_stained_glass", new DefaultBlock());
  BlockID WHITE_STAINED_GLASS =
      RegisterBlock("minecraft:white_stained_glass", new DefaultBlock());
  BlockID YELLOW_STAINED_GLASS =
      RegisterBlock("minecraft:yellow_stained_glass", new DefaultBlock());
  BlockID WHITE_WOOL =
      RegisterBlock("minecraft:white_wool", new DefaultBlock());
  BlockID BLACK_WOOL =
      RegisterBlock("minecraft:black_wool", new DefaultBlock());
  BlockID BLUE_WOOL = RegisterBlock("minecraft:blue_wool", new DefaultBlock());
  BlockID BROWN_WOOL =
      RegisterBlock("minecraft:brown_wool", new DefaultBlock());
  BlockID CYAN_WOOL = RegisterBlock("minecraft:cyan_wool", new DefaultBlock());
  BlockID GRAY_WOOL = RegisterBlock("minecraft:gray_wool", new DefaultBlock());
  BlockID GREEN_WOOL =
      RegisterBlock("minecraft:green_wool", new DefaultBlock());
  BlockID LIGHT_BLUE_WOOL =
      RegisterBlock("minecraft:light_blue_wool", new DefaultBlock());
  BlockID LIGHT_GRAY_WOOL =
      RegisterBlock("minecraft:light_gray_wool", new DefaultBlock());
  BlockID LIME_WOOL = RegisterBlock("minecraft:lime_wool", new DefaultBlock());
  BlockID MAGENTA_WOOL =
      RegisterBlock("minecraft:magenta_wool", new DefaultBlock());
  BlockID ORANGE_WOOL =
      RegisterBlock("minecraft:orange_wool", new DefaultBlock());
  BlockID PINK_WOOL = RegisterBlock("minecraft:pink_wool", new DefaultBlock());
  BlockID PURPLE_WOOL =
      RegisterBlock("minecraft:purple_wool", new DefaultBlock());
  BlockID RED_WOOL = RegisterBlock("minecraft:red_wool", new DefaultBlock());
  BlockID YELLOW_WOOL =
      RegisterBlock("minecraft:yellow_wool", new DefaultBlock());
  BlockID WHITE_TERRACOTTA =
      RegisterBlock("minecraft:white_terracotta", new DefaultBlock());
  BlockID BLACK_TERRACOTTA =
      RegisterBlock("minecraft:black_terracotta", new DefaultBlock());
  BlockID BLUE_TERRACOTTA =
      RegisterBlock("minecraft:blue_terracotta", new DefaultBlock());
  BlockID BROWN_TERRACOTTA =
      RegisterBlock("minecraft:brown_terracotta", new DefaultBlock());
  BlockID CYAN_TERRACOTTA =
      RegisterBlock("minecraft:cyan_terracotta", new DefaultBlock());
  BlockID GRAY_TERRACOTTA =
      RegisterBlock("minecraft:gray_terracotta", new DefaultBlock());
  BlockID GREEN_TERRACOTTA =
      RegisterBlock("minecraft:green_terracotta", new DefaultBlock());
  BlockID LIGHT_BLUE_TERRACOTTA =
      RegisterBlock("minecraft:light_blue_terracotta", new DefaultBlock());
  BlockID LIGHT_GRAY_TERRACOTTA =
      RegisterBlock("minecraft:light_gray_terracotta", new DefaultBlock());
  BlockID LIME_TERRACOTTA =
      RegisterBlock("minecraft:lime_terracotta", new DefaultBlock());
  BlockID MAGENTA_TERRACOTTA =
      RegisterBlock("minecraft:magenta_terracotta", new DefaultBlock());
  BlockID ORANGE_TERRACOTTA =
      RegisterBlock("minecraft:orange_terracotta", new DefaultBlock());
  BlockID PINK_TERRACOTTA =
      RegisterBlock("minecraft:pink_terracotta", new DefaultBlock());
  BlockID PURPLE_TERRACOTTA =
      RegisterBlock("minecraft:purple_terracotta", new DefaultBlock());
  BlockID RED_TERRACOTTA =
      RegisterBlock("minecraft:red_terracotta", new DefaultBlock());
  BlockID YELLOW_TERRACOTTA =
      RegisterBlock("minecraft:yellow_terracotta", new DefaultBlock());
  BlockID WHITE_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:white_glazed_terracotta", new DefaultBlock());
  BlockID BLACK_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:black_glazed_terracotta", new DefaultBlock());
  BlockID BLUE_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:blue_glazed_terracotta", new DefaultBlock());
  BlockID BROWN_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:brown_glazed_terracotta", new DefaultBlock());
  BlockID CYAN_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:cyan_glazed_terracotta", new DefaultBlock());
  BlockID GRAY_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:gray_glazed_terracotta", new DefaultBlock());
  BlockID GREEN_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:green_glazed_terracotta", new DefaultBlock());
  BlockID LIGHT_BLUE_GLAZED_TERRACOTTA = RegisterBlock(
      "minecraft:light_blue_glazed_terracotta", new DefaultBlock());
  BlockID LIGHT_GRAY_GLAZED_TERRACOTTA = RegisterBlock(
      "minecraft:light_gray_glazed_terracotta", new DefaultBlock());
  BlockID LIME_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:lime_glazed_terracotta", new DefaultBlock());
  BlockID MAGENTA_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:magenta_glazed_terracotta", new DefaultBlock());
  BlockID ORANGE_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:orange_glazed_terracotta", new DefaultBlock());
  BlockID PINK_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:pink_glazed_terracotta", new DefaultBlock());
  BlockID PURPLE_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:purple_glazed_terracotta", new DefaultBlock());
  BlockID RED_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:red_glazed_terracotta", new DefaultBlock());
  BlockID YELLOW_GLAZED_TERRACOTTA =
      RegisterBlock("minecraft:yellow_glazed_terracotta", new DefaultBlock());
  BlockID TERRACOTTA =
      RegisterBlock("minecraft:terracotta", new DefaultBlock());
  BlockID WHITE_CONCRETE =
      RegisterBlock("minecraft:white_concrete", new DefaultBlock());
  BlockID BLACK_CONCRETE =
      RegisterBlock("minecraft:black_concrete", new DefaultBlock());
  BlockID BLUE_CONCRETE =
      RegisterBlock("minecraft:blue_concrete", new DefaultBlock());
  BlockID BROWN_CONCRETE =
      RegisterBlock("minecraft:brown_concrete", new DefaultBlock());
  BlockID CYAN_CONCRETE =
      RegisterBlock("minecraft:cyan_concrete", new DefaultBlock());
  BlockID GRAY_CONCRETE =
      RegisterBlock("minecraft:gray_concrete", new DefaultBlock());
  BlockID GREEN_CONCRETE =
      RegisterBlock("minecraft:green_concrete", new DefaultBlock());
  BlockID LIGHT_BLUE_CONCRETE =
      RegisterBlock("minecraft:light_blue_concrete", new DefaultBlock());
  BlockID LIGHT_GRAY_CONCRETE =
      RegisterBlock("minecraft:light_gray_concrete", new DefaultBlock());
  BlockID LIME_CONCRETE =
      RegisterBlock("minecraft:lime_concrete", new DefaultBlock());
  BlockID MAGENTA_CONCRETE =
      RegisterBlock("minecraft:magenta_concrete", new DefaultBlock());
  BlockID ORANGE_CONCRETE =
      RegisterBlock("minecraft:orange_concrete", new DefaultBlock());
  BlockID PINK_CONCRETE =
      RegisterBlock("minecraft:pink_concrete", new DefaultBlock());
  BlockID PURPLE_CONCRETE =
      RegisterBlock("minecraft:purple_concrete", new DefaultBlock());
  BlockID RED_CONCRETE =
      RegisterBlock("minecraft:red_concrete", new DefaultBlock());
  BlockID YELLOW_CONCRETE =
      RegisterBlock("minecraft:yellow_concrete", new DefaultBlock());
  BlockID WHITE_CONCRETE_POWDER =
      RegisterBlock("minecraft:white_concrete_powder",
                    new DefaultBlock());  // FIX POWDERS LATER
  BlockID BLACK_CONCRETE_POWDER =
      RegisterBlock("minecraft:black_concrete_powder", new DefaultBlock());
  BlockID BLUE_CONCRETE_POWDER =
      RegisterBlock("minecraft:blue_concrete_powder", new DefaultBlock());
  BlockID BROWN_CONCRETE_POWDER =
      RegisterBlock("minecraft:brown_concrete_powder", new DefaultBlock());
  BlockID CYAN_CONCRETE_POWDER =
      RegisterBlock("minecraft:cyan_concrete_powder", new DefaultBlock());
  BlockID GRAY_CONCRETE_POWDER =
      RegisterBlock("minecraft:gray_concrete_powder", new DefaultBlock());
  BlockID GREEN_CONCRETE_POWDER =
      RegisterBlock("minecraft:green_concrete_powder", new DefaultBlock());
  BlockID LIGHT_BLUE_CONCRETE_POWDER =
      RegisterBlock("minecraft:light_blue_concrete_powder", new DefaultBlock());
  BlockID LIGHT_GRAY_CONCRETE_POWDER =
      RegisterBlock("minecraft:light_gray_concrete_powder", new DefaultBlock());
  BlockID LIME_CONCRETE_POWDER =
      RegisterBlock("minecraft:lime_concrete_powder", new DefaultBlock());
  BlockID MAGENTA_CONCRETE_POWDER =
      RegisterBlock("minecraft:magenta_concrete_powder", new DefaultBlock());
  BlockID ORANGE_CONCRETE_POWDER =
      RegisterBlock("minecraft:orange_concrete_powder", new DefaultBlock());
  BlockID PINK_CONCRETE_POWDER =
      RegisterBlock("minecraft:pink_concrete_powder", new DefaultBlock());
  BlockID PURPLE_CONCRETE_POWDER =
      RegisterBlock("minecraft:purple_concrete_powder", new DefaultBlock());
  BlockID RED_CONCRETE_POWDER =
      RegisterBlock("minecraft:red_concrete_powder", new DefaultBlock());
  BlockID YELLOW_CONCRETE_POWDER =
      RegisterBlock("minecraft:yellow_concrete_powder", new DefaultBlock());
  BlockID PODZOL = RegisterBlock("minecraft:podzol", new DefaultBlock());
  BlockID MYCELIUM = RegisterBlock("minecraft:mycelium", new DefaultBlock());
  BlockID DIRT_PATH = RegisterBlock("minecraft:dirt_path", new DefaultBlock());
  BlockID COARSE_DIRT =
      RegisterBlock("minecraft:coarse_dirt", new DefaultBlock());
  BlockID ROOTED_DIRT =
      RegisterBlock("minecraft:rooted_dirt", new DefaultBlock());
  BlockID FARMLAND = RegisterBlock("minecraft:farmland", new DefaultBlock());
  BlockID FARMLAND_MOIST =
      RegisterBlock("minecraft:farmland_moist", new DefaultBlock());
  BlockID MUD = RegisterBlock("minecraft:mud", new DefaultBlock());
  BlockID CLAY = RegisterBlock("minecraft:clay", new DefaultBlock());
  BlockID GRAVEL = RegisterBlock(
      "minecraft:gravel", new DefaultBlock());  // Change to falling block later
  BlockID RED_SAND =
      RegisterBlock("minecraft:red_sand",
                    new DefaultBlock());  // Change to falling block later
  BlockID ICE = RegisterBlock("minecraft:ice", new DefaultBlock());
  BlockID BLUE_ICE = RegisterBlock("minecraft:blue_ice", new DefaultBlock());
  BlockID SNOW_BLOCK =
      RegisterBlock("minecraft:snow_block", new DefaultBlock());
  BlockID MOSS_BLOCK =
      RegisterBlock("minecraft:moss_block", new DefaultBlock());
  BlockID GRANITE = RegisterBlock("minecraft:granite", new DefaultBlock());
  BlockID DIORITE = RegisterBlock("minecraft:diorite", new DefaultBlock());
  BlockID ANDESITE = RegisterBlock("minecraft:andesite", new DefaultBlock());
  BlockID CALCITE = RegisterBlock("minecraft:calcite", new DefaultBlock());
  BlockID DRIPSTONE_BLOCK =
      RegisterBlock("minecraft:dripstone_block", new DefaultBlock());
  BlockID PRISMARINE =
      RegisterBlock("minecraft:prismarine", new DefaultBlock());
  BlockID MAGMA = RegisterBlock("minecraft:magma_block",
                                new DefaultBlock());  // special properties
  BlockID OBSIDIAN = RegisterBlock("minecraft:obsidian", new DefaultBlock());
  BlockID CRYING_OBSIDIAN =
      RegisterBlock("minecraft:crying_obsidian", new DefaultBlock());
  BlockID NETHERRACK =
      RegisterBlock("minecraft:netherrack", new DefaultBlock());
  BlockID CRIMSON_NYLIUM =
      RegisterBlock("minecraft:crimson_nylium", new DefaultBlock());
  BlockID WARPED_NYLIUM =
      RegisterBlock("minecraft:warped_nylium", new DefaultBlock());
  BlockID SOUL_SAND = RegisterBlock("minecraft:soul_sand", new DefaultBlock());
  BlockID SOUL_SOIL = RegisterBlock("minecraft:soul_soil", new DefaultBlock());
  BlockID BONE_BLOCK =
      RegisterBlock("minecraft:bone_block", new DefaultBlock());
  BlockID BLACKSTONE =
      RegisterBlock("minecraft:blackstone", new DefaultBlock());
  BlockID BASALT = RegisterBlock("minecraft:basalt", new DefaultBlock());
  BlockID SMOOTH_BASALT =
      RegisterBlock("minecraft:smooth_basalt", new DefaultBlock());
  BlockID END_STONE = RegisterBlock("minecraft:end_stone", new DefaultBlock());
  BlockID COAL_ORE = RegisterBlock("minecraft:coal_ore", new DefaultBlock());
  BlockID DEEPSLATE_COAL_ORE =
      RegisterBlock("minecraft:deepslate_coal_ore", new DefaultBlock());
  BlockID IRON_ORE = RegisterBlock("minecraft:iron_ore", new DefaultBlock());
  BlockID DEEPSLATE_IRON_ORE =
      RegisterBlock("minecraft:deepslate_iron_ore", new DefaultBlock());
  BlockID COPPER_ORE =
      RegisterBlock("minecraft:copper_ore", new DefaultBlock());
  BlockID DEEPSLATE_COPPER_ORE =
      RegisterBlock("minecraft:deepslate_copper_ore", new DefaultBlock());
  BlockID GOLD_ORE = RegisterBlock("minecraft:gold_ore", new DefaultBlock());
  BlockID DEEPSLATE_GOLD_ORE =
      RegisterBlock("minecraft:deepslate_gold_ore", new DefaultBlock());
  BlockID REDSTONE_ORE =
      RegisterBlock("minecraft:redstone_ore", new DefaultBlock());
  BlockID DEEPSLATE_REDSTONE_ORE =
      RegisterBlock("minecraft:deepslate_redstone_ore", new DefaultBlock());
  BlockID EMERALD_ORE =
      RegisterBlock("minecraft:emerald_ore", new DefaultBlock());
  BlockID DEEPSLATE_EMERALD_ORE =
      RegisterBlock("minecraft:deepslate_emerald_ore", new DefaultBlock());
  BlockID LAPIS_ORE = RegisterBlock("minecraft:lapis_ore", new DefaultBlock());
  BlockID DEEPSLATE_LAPIS_ORE =
      RegisterBlock("minecraft:deepslate_lapis_ore", new DefaultBlock());
  BlockID DIAMOND_ORE =
      RegisterBlock("minecraft:diamond_ore", new DefaultBlock());
  BlockID DEEPSLATE_DIAMOND_ORE =
      RegisterBlock("minecraft:deepslate_diamond_ore", new DefaultBlock());
  BlockID NETHER_GOLD_ORE =
      RegisterBlock("minecraft:nether_gold_ore", new DefaultBlock());
  BlockID NETHER_QUARTZ_ORE =
      RegisterBlock("minecraft:nether_quartz_ore", new DefaultBlock());
  BlockID ANCIENT_DEBRIS =
      RegisterBlock("minecraft:ancient_debris", new DefaultBlock());
  BlockID RAW_IRON_BLOCK =
      RegisterBlock("minecraft:raw_iron_block", new DefaultBlock());
  BlockID RAW_COPPER_BLOCK =
      RegisterBlock("minecraft:raw_copper_block", new DefaultBlock());
  BlockID RAW_GOLD_BLOCK =
      RegisterBlock("minecraft:raw_gold_block", new DefaultBlock());
  BlockID GLOWSTONE = RegisterBlock("minecraft:glowstone", new DefaultBlock());
  BlockID AMETHYST_BLOCK =
      RegisterBlock("minecraft:amethyst_block", new DefaultBlock());
  BlockID BUDDING_AMETHYST =
      RegisterBlock("minecraft:budding_amethyst", new DefaultBlock());
  BlockID SPRUCE_LOG =
      RegisterBlock("minecraft:spruce_log", new DefaultBlock());
  BlockID STRIPPED_SPRUCE_LOG =
      RegisterBlock("minecraft:stripped_spruce_log", new DefaultBlock());
  BlockID BIRCH_LOG = RegisterBlock("minecraft:birch_log", new DefaultBlock());
  BlockID STRIPPED_BIRCH_LOG =
      RegisterBlock("minecraft:stripped_birch_log", new DefaultBlock());
  BlockID JUNGLE_LOG =
      RegisterBlock("minecraft:jungle_log", new DefaultBlock());
  BlockID STRIPPED_JUNGLE_LOG =
      RegisterBlock("minecraft:stripped_jungle_log", new DefaultBlock());
  BlockID ACACIA_LOG =
      RegisterBlock("minecraft:acacia_log", new DefaultBlock());
  BlockID STRIPPED_ACACIA_LOG =
      RegisterBlock("minecraft:stripped_acacia_log", new DefaultBlock());
  BlockID DARK_OAK_LOG =
      RegisterBlock("minecraft:dark_oak_log", new DefaultBlock());
  BlockID STRIPPED_DARK_OAK_LOG =
      RegisterBlock("minecraft:stripped_dark_oak_log", new DefaultBlock());
  BlockID CHERRY_LOG =
      RegisterBlock("minecraft:cherry_log", new DefaultBlock());
  BlockID STRIPPED_CHERRY_LOG =
      RegisterBlock("minecraft:stripped_cherry_log", new DefaultBlock());
  BlockID MANGROVE_LOG =
      RegisterBlock("minecraft:mangrove_log", new DefaultBlock());
  BlockID STRIPPED_MANGROVE_LOG =
      RegisterBlock("minecraft:stripped_mangrove_log", new DefaultBlock());
  BlockID CRIMSON_STEM =
      RegisterBlock("minecraft:crimson_stem", new DefaultBlock());
  BlockID STRIPPED_CRIMSON_STEM =
      RegisterBlock("minecraft:stripped_crimson_stem", new DefaultBlock());
  BlockID WARPED_STEM =
      RegisterBlock("minecraft:warped_stem", new DefaultBlock());
  BlockID STRIPPED_WARPED_STEM =
      RegisterBlock("minecraft:stripped_warped_stem", new DefaultBlock());
  BlockID MANGROVE_ROOTS =
      RegisterBlock("minecraft:mangrove_roots", new DefaultBlock());
  BlockID MUDDY_MANGROVE_ROOTS =
      RegisterBlock("minecraft:muddy_mangrove_roots", new DefaultBlock());
  BlockID MUSHROOM_STEM =
      RegisterBlock("minecraft:mushroom_stem", new DefaultBlock());
  BlockID BROWN_MUSHROOM_BLOCK =
      RegisterBlock("minecraft:brown_mushroom_block", new DefaultBlock());
  BlockID RED_MUSHROOM_BLOCK =
      RegisterBlock("minecraft:red_mushroom_block", new DefaultBlock());
  BlockID NETHER_WART_BLOCK =
      RegisterBlock("minecraft:nether_wart_block", new DefaultBlock());
  BlockID WARPED_WART_BLOCK =
      RegisterBlock("minecraft:warped_wart_block", new DefaultBlock());
  BlockID SHROOMLIGHT =
      RegisterBlock("minecraft:shroomlight", new DefaultBlock());
  BlockID DRIED_KELP_BLOCK =
      RegisterBlock("minecraft:dried_kelp_block", new DefaultBlock());
  BlockID TUBE_CORAL_BLOCK =
      RegisterBlock("minecraft:tube_coral_block", new DefaultBlock());
  BlockID BRAIN_CORAL_BLOCK =
      RegisterBlock("minecraft:brain_coral_block", new DefaultBlock());
  BlockID BUBBLE_CORAL_BLOCK =
      RegisterBlock("minecraft:bubble_coral_block", new DefaultBlock());
  BlockID FIRE_CORAL_BLOCK =
      RegisterBlock("minecraft:fire_coral_block", new DefaultBlock());
  BlockID HORN_CORAL_BLOCK =
      RegisterBlock("minecraft:horn_coral_block", new DefaultBlock());
  BlockID DEAD_TUBE_CORAL_BLOCK =
      RegisterBlock("minecraft:dead_tube_coral_block", new DefaultBlock());
  BlockID DEAD_BRAIN_CORAL_BLOCK =
      RegisterBlock("minecraft:dead_brain_coral_block", new DefaultBlock());
  BlockID DEAD_BUBBLE_CORAL_BLOCK =
      RegisterBlock("minecraft:dead_bubble_coral_block", new DefaultBlock());
  BlockID DEAD_FIRE_CORAL_BLOCK =
      RegisterBlock("minecraft:dead_fire_coral_block", new DefaultBlock());
  BlockID DEAD_HORN_CORAL_BLOCK =
      RegisterBlock("minecraft:dead_horn_coral_block", new DefaultBlock());
  BlockID SPONGE = RegisterBlock("minecraft:sponge", new DefaultBlock());
  BlockID WET_SPONGE =
      RegisterBlock("minecraft:wet_sponge", new DefaultBlock());
  BlockID MELON = RegisterBlock("minecraft:melon", new DefaultBlock());
  BlockID PUMPKIN = RegisterBlock("minecraft:pumpkin", new DefaultBlock());
  BlockID CARVED_PUMPKIN =
      RegisterBlock("minecraft:carved_pumpkin", new DefaultBlock());
  BlockID JACK_O_LANTERN =
      RegisterBlock("minecraft:jack_o_lantern", new DefaultBlock());
  BlockID HAY_BALE = RegisterBlock("minecraft:hay_block", new DefaultBlock());
  BlockID BEE_NEST = RegisterBlock("minecraft:bee_nest", new DefaultBlock());
  BlockID HONEYCOMB_BLOCK =
      RegisterBlock("minecraft:honeycomb_block", new DefaultBlock());
  BlockID OCHRE_FROGLIGHT =
      RegisterBlock("minecraft:ochre_froglight", new DefaultBlock());
  BlockID VERDANT_FROGLIGHT =
      RegisterBlock("minecraft:verdant_froglight", new DefaultBlock());
  BlockID PEARLESCENT_FROGLIGHT =
      RegisterBlock("minecraft:pearlescent_froglight", new DefaultBlock());
  BlockID SCULK = RegisterBlock("minecraft:sculk", new DefaultBlock());
  BlockID SCULK_CATALYST =
      RegisterBlock("minecraft:sculk_catalyst", new DefaultBlock());
  BlockID BEDROCK = RegisterBlock("minecraft:bedrock", new DefaultBlock());
  BlockID SPRUCE_LEAVES =
      RegisterBlock("minecraft:spruce_leaves", new DefaultBlock());
  BlockID BIRCH_LEAVES =
      RegisterBlock("minecraft:birch_leaves", new DefaultBlock());
  BlockID JUNGLE_LEAVES =
      RegisterBlock("minecraft:jungle_leaves", new DefaultBlock());
  BlockID ACACIA_LEAVES =
      RegisterBlock("minecraft:acacia_leaves", new DefaultBlock());
  BlockID DARK_OAK_LEAVES =
      RegisterBlock("minecraft:dark_oak_leaves", new DefaultBlock());
  BlockID MANGROVE_LEAVES =
      RegisterBlock("minecraft:mangrove_leaves", new DefaultBlock());
  BlockID CHERRY_LEAVES =
      RegisterBlock("minecraft:cherry_leaves", new DefaultBlock());
  BlockID AZALEA_LEAVES =
      RegisterBlock("minecraft:azalea_leaves", new DefaultBlock());
  BlockID FLOWERING_AZALEA_LEAVES =
      RegisterBlock("minecraft:flowering_azalea_leaves", new DefaultBlock());
  BlockID OAK_LEAF = RegisterBlock("minecraft:oak_leaves", new DefaultBlock());
  BlockID SEA_LANTERN =
      RegisterBlock("minecraft:sea_lantern", new DefaultBlock());
  BlockID REDSTONE_LAMP =
      RegisterBlock("minecraft:redstone_lamp", new DefaultBlock());
  BlockID CRAFTING_TABLE =
      RegisterBlock("minecraft:crafting_table", new DefaultBlock());
  BlockID COPPER_BULB =
      RegisterBlock("minecraft:copper_bulb", new DefaultBlock());
  BlockID EXPOSED_COPPER_BULB =
      RegisterBlock("minecraft:exposed_copper_bulb", new DefaultBlock());
  BlockID OXIDIZED_COPPER_BULB =
      RegisterBlock("minecraft:oxidized_copper_bulb", new DefaultBlock());
  BlockID WEATHERED_COPPER_BULB =
      RegisterBlock("minecraft:weathered_copper_bulb", new DefaultBlock());
  BlockID COPPER_GRATE =
      RegisterBlock("minecraft:copper_grate", new DefaultBlock());
  BlockID EXPOSED_COPPER_GRATE =
      RegisterBlock("minecraft:exposed_copper_grate", new DefaultBlock());
  BlockID OXIDIZED_COPPER_GRATE =
      RegisterBlock("minecraft:oxidized_copper_grate", new DefaultBlock());
  BlockID WEATHERED_COPPER_GRATE =
      RegisterBlock("minecraft:weathered_copper_grate", new DefaultBlock());
  BlockID CUT_COPPER =
      RegisterBlock("minecraft:cut_copper", new DefaultBlock());
  BlockID EXPOSED_CUT_COPPER =
      RegisterBlock("minecraft:exposed_cut_copper", new DefaultBlock());
  BlockID OXIDIZED_CUT_COPPER =
      RegisterBlock("minecraft:oxidized_cut_copper", new DefaultBlock());
  BlockID WEATHERED_CUT_COPPER =
      RegisterBlock("minecraft:weathered_cut_copper", new DefaultBlock());
  BlockID CHISELED_COPPER =
      RegisterBlock("minecraft:chiseled_copper", new DefaultBlock());
  BlockID EXPOSED_CHISELED_COPPER =
      RegisterBlock("minecraft:exposed_chiseled_copper", new DefaultBlock());
  BlockID OXIDIZED_CHISELED_COPPER =
      RegisterBlock("minecraft:oxidized_chiseled_copper", new DefaultBlock());
  BlockID WEATHERED_CHISELED_COPPER =
      RegisterBlock("minecraft:weathered_chiseled_copper", new DefaultBlock());
  BlockID WARPED_HYPHAE =
      RegisterBlock("minecraft:warped_hyphae", new DefaultBlock());
  BlockID STRIPPED_WARPED_HYPHAE =
      RegisterBlock("minecraft:stripped_warped_hyphae", new DefaultBlock());
  BlockID CRIMSON_HYPHAE =
      RegisterBlock("minecraft:crimson_hyphae", new DefaultBlock());
  BlockID STRIPPED_CRIMSON_HYPHAE =
      RegisterBlock("minecraft:stripped_crimson_hyphae", new DefaultBlock());
  BlockID MOSSY_COBBLESTONE =
      RegisterBlock("minecraft:mossy_cobblestone", new DefaultBlock());
  BlockID DEEPSLATE = RegisterBlock("minecraft:deepslate", new DefaultBlock());
  BlockID COBBLED_DEEPSLATE =
      RegisterBlock("minecraft:cobbled_deepslate", new DefaultBlock());
  BlockID CHISELED_DEEPSLATE =
      RegisterBlock("minecraft:chiseled_deepslate", new DefaultBlock());
  BlockID POLISHED_DEEPSLATE =
      RegisterBlock("minecraft:polished_deepslate", new DefaultBlock());
  BlockID DEEPSLATE_TILES =
      RegisterBlock("minecraft:deepslate_tiles", new DefaultBlock());
  BlockID CRACKED_DEEPSLATE_TILES =
      RegisterBlock("minecraft:deepslate_tiles", new DefaultBlock());
  BlockID REINFORCED_DEEPSLATE =
      RegisterBlock("minecraft:reinforced_deepslate", new DefaultBlock());
  BlockID TUFF = RegisterBlock("minecraft:tuff", new DefaultBlock());
  BlockID CHISELED_TUFF =
      RegisterBlock("minecraft:chiseled_tuff", new DefaultBlock());
  BlockID POLISHED_TUFF =
      RegisterBlock("minecraft:polished_tuff", new DefaultBlock());
  BlockID PACKED_MUD =
      RegisterBlock("minecraft:packed_mud", new DefaultBlock());
  BlockID SANDSTONE = RegisterBlock("minecraft:sandstone", new DefaultBlock());
  BlockID CHISELED_SANDSTONE =
      RegisterBlock("minecraft:chiseled_sandstone", new DefaultBlock());
  BlockID SMOOTH_SANDSTONE =
      RegisterBlock("minecraft:smooth_sandstone", new DefaultBlock());
  BlockID CUT_SANDSTONE =
      RegisterBlock("minecraft:cut_sandstone", new DefaultBlock());
  BlockID RED_SANDSTONE =
      RegisterBlock("minecraft:red_sandstone", new DefaultBlock());
  BlockID CHISELED_RED_SANDSTONE =
      RegisterBlock("minecraft:chiseled_red_sandstone", new DefaultBlock());
  BlockID SMOOTH_RED_SANDSTONE =
      RegisterBlock("minecraft:smooth_red_sandstone", new DefaultBlock());
  BlockID CUT_RED_SANDSTONE =
      RegisterBlock("minecraft:cut_red_sandstone", new DefaultBlock());
  BlockID DARK_PRISMARINE =
      RegisterBlock("minecraft:dark_prismarine", new DefaultBlock());
  BlockID PURPUR_BLOCK =
      RegisterBlock("minecraft:purpur_block", new DefaultBlock());
  BlockID PURPUR_PILLAR =
      RegisterBlock("minecraft:purpur_pillar", new DefaultBlock());
  BlockID COAL_BLOCK =
      RegisterBlock("minecraft:coal_block", new DefaultBlock());
  BlockID IRON_BLOCK =
      RegisterBlock("minecraft:iron_block", new DefaultBlock());
  BlockID GOLD_BLOCK =
      RegisterBlock("minecraft:gold_block", new DefaultBlock());
  BlockID REDSTONE_BLOCK =
      RegisterBlock("minecraft:redstone_block", new DefaultBlock());
  BlockID EMERALD_BLOCK =
      RegisterBlock("minecraft:emerald_block", new DefaultBlock());
  BlockID LAPIS_BLOCK =
      RegisterBlock("minecraft:lapis_block", new DefaultBlock());
  BlockID DIAMOND_BLOCK =
      RegisterBlock("minecraft:diamond_block", new DefaultBlock());
  BlockID NETHERITE_BLOCK =
      RegisterBlock("minecraft:netherite_block", new DefaultBlock());
  BlockID QUARTZ_BLOCK =
      RegisterBlock("minecraft:quartz_block", new DefaultBlock());
  BlockID CHISELED_QUARTZ_BLOCK =
      RegisterBlock("minecraft:chiseled_quartz_block", new DefaultBlock());
  BlockID QUARTZ_PILLAR =
      RegisterBlock("minecraft:quartz_pillar", new DefaultBlock());
  BlockID SMOOTH_QUARTZ =
      RegisterBlock("minecraft:smooth_quartz", new DefaultBlock());

  BlockID ANVIL = RegisterBlock("minecraft:anvil", new DefaultBlock());

  void Initialize();

  BlockList();
  ~BlockList();

  Block* GetBlockType(BlockID id);

  const BlockModel& GetBlockModel(BlockID) const;
  const BlockProperties& GetBlockProperties(BlockID) const;

 private:
  BlockID RegisterBlock(std::string blockName, Block* block);

  void InitializeBlockModels();
  void AddAssets(std::string namespaceIn);

  int block_type_count_ = 0;
};

extern BlockList g_blocks;
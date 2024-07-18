#pragma once
#include <unordered_map>
#include "Block.h"
#include "Material/BlockMaterial.h"
#include "Texture/BlockTexture.h"


class BlockList{
private:
	std::unordered_map<std::string, BlockID> BlockIDNameData;
	int BlockTypeCount = 0;

	void InitializeTextures();
	void InitializeTextureV2();
	void InitializeBlockModels();
public:
	std::vector<Block*> BlockTypeData;

	TextureArray BlockTextureArray;
	TextureAtlas BlockTextureAtlas;


	BlockID AIR = RegisterBlock("air", new MaterialNone(), true, false, false);
	BlockID NULL_BLOCK = RegisterBlock("null", new MaterialNone(), false, false, false);
	BlockID STONE = RegisterBlock("stone", new MaterialNone(), false, true, false);
	BlockID DIRT = RegisterBlock("dirt", new MaterialDirt(), false, true, false);
	BlockID WATER = RegisterBlock("water", new MaterialFluid(5), true, false, true);
	BlockID LAVA = RegisterBlock("lava", new MaterialFluid(5), true, false, true);
	BlockID GRASS = RegisterBlock("grass_block", new MaterialGrass(0.1, 0.1), false, true, false);
	BlockID SAND = RegisterBlock("sand", new MaterialGravity(), false, true, false);
	BlockID OAK_LOG = RegisterBlock("oak_log", new MaterialNone(), false, true, false);
	BlockID STRIPPED_OAK_LOG = RegisterBlock("stripped_oak_log", new MaterialNone(), false, true, false);
	BlockID OAK_PLANKS = RegisterBlock("oak_planks", new MaterialNone(), false, true, false);
	BlockID ACACIA_PLANKS = RegisterBlock("acacia_planks", new MaterialNone(), false, true, false);
	BlockID BAMBOO_PLANKS = RegisterBlock("bamboo_planks", new MaterialNone(), false, true, false);
	BlockID BIRCH_PLANKS = RegisterBlock("birch_planks", new MaterialNone(), false, true, false);
	BlockID CHERRY_PLANKS = RegisterBlock("cherry_planks", new MaterialNone(), false, true, false);
	BlockID CRIMSON_PLANKS = RegisterBlock("crimson_planks", new MaterialNone(), false, true, false);
	BlockID DARK_OAK_PLANKS = RegisterBlock("dark_oak_planks", new MaterialNone(), false, true, false);
	BlockID JUNGLE_PLANKS = RegisterBlock("jungle_planks", new MaterialNone(), false, true, false);
	BlockID MANGROVE_PLANKS = RegisterBlock("mangrove_planks", new MaterialNone(), false, true, false);
	BlockID SPRUCE_PLANKS = RegisterBlock("spruce_planks", new MaterialNone(), false, true, false);
	BlockID WARPED_PLANKS = RegisterBlock("warped_planks", new MaterialNone(), false, true, false);
	BlockID OAK_WOOD = RegisterBlock("oak_wood", new MaterialNone(), false, true, false);
	BlockID ACACIA_WOOD = RegisterBlock("acacia_wood", new MaterialNone(), false, true, false);
	BlockID BIRCH_WOOD = RegisterBlock("birch_wood", new MaterialNone(), false, true, false);
	BlockID CHERRY_WOOD = RegisterBlock("cherry_wood", new MaterialNone(), false, true, false);
	BlockID DARK_OAK_WOOD = RegisterBlock("dark_oak_wood", new MaterialNone(), false, true, false);
	BlockID JUNGLE_WOOD = RegisterBlock("jungle_wood", new MaterialNone(), false, true, false);
	BlockID MANGROVE_WOOD = RegisterBlock("mangrove_wood", new MaterialNone(), false, true, false);
	BlockID SPRUCE_WOOD = RegisterBlock("spruce_wood", new MaterialNone(), false, true, false);
	BlockID STRIPPED_OAK_WOOD = RegisterBlock("stripped_oak_wood", new MaterialNone(), false, true, false);
	BlockID STRIPPED_ACACIA_WOOD = RegisterBlock("stripped_acacia_wood", new MaterialNone(), false, true, false);
	BlockID STRIPPED_BIRCH_WOOD = RegisterBlock("stripped_birch_wood", new MaterialNone(), false, true, false);
	BlockID STRIPPED_CHERRY_WOOD = RegisterBlock("stripped_cherry_wood", new MaterialNone(), false, true, false);
	BlockID STRIPPED_DARK_OAK_WOOD = RegisterBlock("stripped_dark_oak_wood", new MaterialNone(), false, true, false);
	BlockID STRIPPED_JUNGLE_WOOD = RegisterBlock("stripped_jungle_wood", new MaterialNone(), false, true, false);
	BlockID STRIPPED_MANGROVE_WOOD = RegisterBlock("stripped_mangrove_wood", new MaterialNone(), false, true, false);
	BlockID STRIPPED_SPRUCE_WOOD = RegisterBlock("stripped_spruce_wood", new MaterialNone(), false, true, false);
	BlockID BRICKS = RegisterBlock("bricks", new MaterialNone(), false, true, false);
	BlockID STONE_BRICKS = RegisterBlock("stone_bricks", new MaterialNone(), false, true, false);
	BlockID CRACKED_STONE_BRICKS = RegisterBlock("cracked_stone_bricks", new MaterialNone(), false, true, false);
	BlockID CHISELED_STONE_BRICKS = RegisterBlock("chiseled_stone_bricks", new MaterialNone(), false, true, false);
	BlockID MOSSY_STONE_BRICKS = RegisterBlock("mossy_stone_bricks", new MaterialNone(), false, true, false);
	BlockID DEEPSLATE_BRICKS = RegisterBlock("deepslate_bricks", new MaterialNone(), false, true, false);
	BlockID CRACKED_DEEPSLATE_BRICKS = RegisterBlock("cracked_deepslate_bricks", new MaterialNone(), false, true, false);
	BlockID TUFF_BRICKS = RegisterBlock("tuff_bricks", new MaterialNone(), false, true, false);
	BlockID CHISELED_TUFF_BRICKS = RegisterBlock("chiseled_tuff_bricks", new MaterialNone(), false, true, false);
	BlockID MUD_BRICKS = RegisterBlock("mud_bricks", new MaterialNone(), false, true, false);
	BlockID PRISMARINE_BRICKS = RegisterBlock("prismarine_bricks", new MaterialNone(), false, true, false);
	BlockID NETHER_BRICKS = RegisterBlock("nether_bricks", new MaterialNone(), false, true, false);
	BlockID CRACKED_NETHER_BRICKS = RegisterBlock("cracked_nether_bricks", new MaterialNone(), false, true, false);
	BlockID CHISELED_NETHER_BRICKS = RegisterBlock("chiseled_nether_bricks", new MaterialNone(), false, true, false);
	BlockID RED_NETHER_BRICKS = RegisterBlock("red_nether_bricks", new MaterialNone(), false, true, false);
	BlockID POLISHED_BLACKSTONE_BRICK = RegisterBlock("polished_blackstone_bricks", new MaterialNone(), false, true, false);
	BlockID CRACKED_POLISHED_BLACKSTONE_BRICK = RegisterBlock("cracked_polished_blackstone_bricks", new MaterialNone(), false, true, false);
	BlockID END_STONE_BRICKS = RegisterBlock("end_stone_bricks", new MaterialNone(), false, true, false);
	BlockID COBBLESTONE = RegisterBlock("cobblestone", new MaterialNone(), false, true, false);
	BlockID BLUE_STAINED_GLASS = RegisterBlock("blue_stained_glass", new MaterialNone(), true, true, false);
	BlockID GREEN_STAINED_GLASS = RegisterBlock("green_stained_glass", new MaterialNone(), true, true, false);
	BlockID BLACK_STAINED_GLASS = RegisterBlock("black_stained_glass", new MaterialNone(), true, true, false);
	BlockID BROWN_STAINED_GLASS = RegisterBlock("brown_stained_glass", new MaterialNone(), true, true, false);
	BlockID CYAN_STAINED_GLASS = RegisterBlock("cyan_stained_glass", new MaterialNone(), true, true, false);
	BlockID GRAY_STAINED_GLASS = RegisterBlock("gray_stained_glass", new MaterialNone(), true, true, false);
	BlockID LIGHT_BLUE_STAINED_GLASS = RegisterBlock("light_blue_stained_glass", new MaterialNone(), true, true, false);
	BlockID LIGHT_GRAY_STAINED_GLASS = RegisterBlock("light_gray_stained_glass", new MaterialNone(), true, true, false);
	BlockID LIME_STAINED_GLASS = RegisterBlock("lime_stained_glass", new MaterialNone(), true, true, false);
	BlockID MAGENTA_STAINED_GLASS = RegisterBlock("magenta_stained_glass", new MaterialNone(), true, true, false);
	BlockID ORANGE_STAINED_GLASS = RegisterBlock("orange_stained_glass", new MaterialNone(), true, true, false);
	BlockID PINK_STAINED_GLASS = RegisterBlock("pink_stained_glass", new MaterialNone(), true, true, false);
	BlockID PURPLE_STAINED_GLASS = RegisterBlock("purple_stained_glass", new MaterialNone(), true, true, false);
	BlockID RED_STAINED_GLASS = RegisterBlock("red_stained_glass", new MaterialNone(), true, true, false);
	BlockID WHITE_STAINED_GLASS = RegisterBlock("white_stained_glass", new MaterialNone(), true, true, false);
	BlockID YELLOW_STAINED_GLASS = RegisterBlock("yellow_stained_glass", new MaterialNone(), true, true, false);
	BlockID WHITE_WOOL = RegisterBlock("white_wool", new MaterialNone(), false, true, false);
	BlockID BLACK_WOOL = RegisterBlock("black_wool", new MaterialNone(), false, true, false);
	BlockID BLUE_WOOL = RegisterBlock("blue_wool", new MaterialNone(), false, true, false);
	BlockID BROWN_WOOL = RegisterBlock("brown_wool", new MaterialNone(), false, true, false);
	BlockID CYAN_WOOL = RegisterBlock("cyan_wool", new MaterialNone(), false, true, false);
	BlockID GRAY_WOOL = RegisterBlock("gray_wool", new MaterialNone(), false, true, false);
	BlockID GREEN_WOOL = RegisterBlock("green_wool", new MaterialNone(), false, true, false);
	BlockID LIGHT_BLUE_WOOL = RegisterBlock("light_blue_wool", new MaterialNone(), false, true, false);
	BlockID LIGHT_GRAY_WOOL = RegisterBlock("light_gray_wool", new MaterialNone(), false, true, false);
	BlockID LIME_WOOL = RegisterBlock("lime_wool", new MaterialNone(), false, true, false);
	BlockID MAGENTA_WOOL = RegisterBlock("magenta_wool", new MaterialNone(), false, true, false);
	BlockID ORANGE_WOOL = RegisterBlock("orange_wool", new MaterialNone(), false, true, false);
	BlockID PINK_WOOL = RegisterBlock("pink_wool", new MaterialNone(), false, true, false);
	BlockID PURPLE_WOOL = RegisterBlock("purple_wool", new MaterialNone(), false, true, false);
	BlockID RED_WOOL = RegisterBlock("red_wool", new MaterialNone(), false, true, false);
	BlockID YELLOW_WOOL = RegisterBlock("yellow_wool", new MaterialNone(), false, true, false);
	BlockID WHITE_TERRACOTTA = RegisterBlock("white_terracotta", new MaterialNone(), false, true, false);
	BlockID BLACK_TERRACOTTA = RegisterBlock("black_terracotta", new MaterialNone(), false, true, false);
	BlockID BLUE_TERRACOTTA = RegisterBlock("blue_terracotta", new MaterialNone(), false, true, false);
	BlockID BROWN_TERRACOTTA = RegisterBlock("brown_terracotta", new MaterialNone(), false, true, false);
	BlockID CYAN_TERRACOTTA = RegisterBlock("cyan_terracotta", new MaterialNone(), false, true, false);
	BlockID GRAY_TERRACOTTA = RegisterBlock("gray_terracotta", new MaterialNone(), false, true, false);
	BlockID GREEN_TERRACOTTA = RegisterBlock("green_terracotta", new MaterialNone(), false, true, false);
	BlockID LIGHT_BLUE_TERRACOTTA = RegisterBlock("light_blue_terracotta", new MaterialNone(), false, true, false);
	BlockID LIGHT_GRAY_TERRACOTTA = RegisterBlock("light_gray_terracotta", new MaterialNone(), false, true, false);
	BlockID LIME_TERRACOTTA = RegisterBlock("lime_terracotta", new MaterialNone(), false, true, false);
	BlockID MAGENTA_TERRACOTTA = RegisterBlock("magenta_terracotta", new MaterialNone(), false, true, false);
	BlockID ORANGE_TERRACOTTA = RegisterBlock("orange_terracotta", new MaterialNone(), false, true, false);
	BlockID PINK_TERRACOTTA = RegisterBlock("pink_terracotta", new MaterialNone(), false, true, false);
	BlockID PURPLE_TERRACOTTA = RegisterBlock("purple_terracotta", new MaterialNone(), false, true, false);
	BlockID RED_TERRACOTTA = RegisterBlock("red_terracotta", new MaterialNone(), false, true, false);
	BlockID YELLOW_TERRACOTTA = RegisterBlock("yellow_terracotta", new MaterialNone(), false, true, false);
	BlockID WHITE_GLAZED_TERRACOTTA = RegisterBlock("white_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID BLACK_GLAZED_TERRACOTTA = RegisterBlock("black_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID BLUE_GLAZED_TERRACOTTA = RegisterBlock("blue_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID BROWN_GLAZED_TERRACOTTA = RegisterBlock("brown_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID CYAN_GLAZED_TERRACOTTA = RegisterBlock("cyan_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID GRAY_GLAZED_TERRACOTTA = RegisterBlock("gray_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID GREEN_GLAZED_TERRACOTTA = RegisterBlock("green_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID LIGHT_BLUE_GLAZED_TERRACOTTA = RegisterBlock("light_blue_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID LIGHT_GRAY_GLAZED_TERRACOTTA = RegisterBlock("light_gray_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID LIME_GLAZED_TERRACOTTA = RegisterBlock("lime_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID MAGENTA_GLAZED_TERRACOTTA = RegisterBlock("magenta_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID ORANGE_GLAZED_TERRACOTTA = RegisterBlock("orange_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID PINK_GLAZED_TERRACOTTA = RegisterBlock("pink_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID PURPLE_GLAZED_TERRACOTTA = RegisterBlock("purple_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID RED_GLAZED_TERRACOTTA = RegisterBlock("red_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID YELLOW_GLAZED_TERRACOTTA = RegisterBlock("yellow_glazed_terracotta", new MaterialNone(), false, true, false);
	BlockID TERRACOTTA = RegisterBlock("terracotta", new MaterialNone(), false, true, false);
	BlockID WHITE_CONCRETE = RegisterBlock("white_concrete", new MaterialNone(), false, true, false);
	BlockID BLACK_CONCRETE = RegisterBlock("black_concrete", new MaterialNone(), false, true, false);
	BlockID BLUE_CONCRETE = RegisterBlock("blue_concrete", new MaterialNone(), false, true, false);
	BlockID BROWN_CONCRETE = RegisterBlock("brown_concrete", new MaterialNone(), false, true, false);
	BlockID CYAN_CONCRETE = RegisterBlock("cyan_concrete", new MaterialNone(), false, true, false);
	BlockID GRAY_CONCRETE = RegisterBlock("gray_concrete", new MaterialNone(), false, true, false);
	BlockID GREEN_CONCRETE = RegisterBlock("green_concrete", new MaterialNone(), false, true, false);
	BlockID LIGHT_BLUE_CONCRETE = RegisterBlock("light_blue_concrete", new MaterialNone(), false, true, false);
	BlockID LIGHT_GRAY_CONCRETE = RegisterBlock("light_gray_concrete", new MaterialNone(), false, true, false);
	BlockID LIME_CONCRETE = RegisterBlock("lime_concrete", new MaterialNone(), false, true, false);
	BlockID MAGENTA_CONCRETE = RegisterBlock("magenta_concrete", new MaterialNone(), false, true, false);
	BlockID ORANGE_CONCRETE = RegisterBlock("orange_concrete", new MaterialNone(), false, true, false);
	BlockID PINK_CONCRETE = RegisterBlock("pink_concrete", new MaterialNone(), false, true, false);
	BlockID PURPLE_CONCRETE = RegisterBlock("purple_concrete", new MaterialNone(), false, true, false);
	BlockID RED_CONCRETE = RegisterBlock("red_concrete", new MaterialNone(), false, true, false);
	BlockID YELLOW_CONCRETE = RegisterBlock("yellow_concrete", new MaterialNone(), false, true, false);
	BlockID WHITE_CONCRETE_POWDER = RegisterBlock("white_concrete_powder", new MaterialNone(), false, true, false); //FIX POWDERS LATER
	BlockID BLACK_CONCRETE_POWDER = RegisterBlock("black_concrete_powder", new MaterialNone(), false, true, false);
	BlockID BLUE_CONCRETE_POWDER = RegisterBlock("blue_concrete_powder", new MaterialNone(), false, true, false);
	BlockID BROWN_CONCRETE_POWDER = RegisterBlock("brown_concrete_powder", new MaterialNone(), false, true, false);
	BlockID CYAN_CONCRETE_POWDER = RegisterBlock("cyan_concrete_powder", new MaterialNone(), false, true, false);
	BlockID GRAY_CONCRETE_POWDER = RegisterBlock("gray_concrete_powder", new MaterialNone(), false, true, false);
	BlockID GREEN_CONCRETE_POWDER = RegisterBlock("green_concrete_powder", new MaterialNone(), false, true, false);
	BlockID LIGHT_BLUE_CONCRETE_POWDER = RegisterBlock("light_blue_concrete_powder", new MaterialNone(), false, true, false);
	BlockID LIGHT_GRAY_CONCRETE_POWDER = RegisterBlock("light_gray_concrete_powder", new MaterialNone(), false, true, false);
	BlockID LIME_CONCRETE_POWDER = RegisterBlock("lime_concrete_powder", new MaterialNone(), false, true, false);
	BlockID MAGENTA_CONCRETE_POWDER = RegisterBlock("magenta_concrete_powder", new MaterialNone(), false, true, false);
	BlockID ORANGE_CONCRETE_POWDER = RegisterBlock("orange_concrete_powder", new MaterialNone(), false, true, false);
	BlockID PINK_CONCRETE_POWDER = RegisterBlock("pink_concrete_powder", new MaterialNone(), false, true, false);
	BlockID PURPLE_CONCRETE_POWDER = RegisterBlock("purple_concrete_powder", new MaterialNone(), false, true, false);
	BlockID RED_CONCRETE_POWDER = RegisterBlock("red_concrete_powder", new MaterialNone(), false, true, false);
	BlockID YELLOW_CONCRETE_POWDER = RegisterBlock("yellow_concrete_powder", new MaterialNone(), false, true, false);
	BlockID PODZOL = RegisterBlock("podzol", new MaterialNone(), false, true, false);
	BlockID MYCELIUM = RegisterBlock("mycelium", new MaterialNone(), false, true, false);
	BlockID DIRT_PATH = RegisterBlock("dirt_path", new MaterialNone(), false, true, false);
	BlockID COARSE_DIRT = RegisterBlock("coarse_dirt", new MaterialNone(), false, true, false);
	BlockID ROOTED_DIRT = RegisterBlock("rooted_dirt", new MaterialNone(), false, true, false);
	BlockID FARMLAND = RegisterBlock("farmland", new MaterialNone(), false, true, false);
	BlockID FARMLAND_MOIST = RegisterBlock("farmland_moist", new MaterialNone(), false, true, false);
	BlockID MUD = RegisterBlock("mud", new MaterialNone(), false, true, false);
	BlockID CLAY = RegisterBlock("clay", new MaterialNone(), false, true, false);
	BlockID GRAVEL = RegisterBlock("gravel", new MaterialNone(), false, true, false); //Change to falling block later
	BlockID RED_SAND = RegisterBlock("red_sand", new MaterialNone(), false, true, false); //Change to falling block later
	BlockID ICE = RegisterBlock("ice", new MaterialNone(), true, true, false);
	BlockID BLUE_ICE = RegisterBlock("blue_ice", new MaterialNone(), false, true, false);
	BlockID SNOW_BLOCK = RegisterBlock("snow_block", new MaterialNone(), false, true, false);
	BlockID MOSS_BLOCK = RegisterBlock("moss_block", new MaterialNone(), false, true, false);
	BlockID GRANITE = RegisterBlock("granite", new MaterialNone(), false, true, false);
	BlockID DIORITE = RegisterBlock("diorite", new MaterialNone(), false, true, false);
	BlockID ANDESITE = RegisterBlock("andesite", new MaterialNone(), false, true, false);
	BlockID CALCITE = RegisterBlock("calcite", new MaterialNone(), false, true, false);
	BlockID DRIPSTONE_BLOCK = RegisterBlock("dripstone_block", new MaterialNone(), false, true, false);
	BlockID PRISMARINE = RegisterBlock("prismarine", new MaterialNone(), false, true, false);
	BlockID MAGMA = RegisterBlock("magma_block", new MaterialNone(), false, true, false); //special properties
	BlockID OBSIDIAN = RegisterBlock("obsidian", new MaterialNone(), false, true, false);
	BlockID CRYING_OBSIDIAN = RegisterBlock("crying_obsidian", new MaterialNone(), false, true, false);
	BlockID NETHERRACK = RegisterBlock("netherrack", new MaterialNone(), false, true, false);
	BlockID CRIMSON_NYLIUM = RegisterBlock("crimson_nylium", new MaterialNone(), false, true, false);
	BlockID WARPED_NYLIUM = RegisterBlock("warped_nylium", new MaterialNone(), false, true, false);
	BlockID SOUL_SAND = RegisterBlock("soul_sand", new MaterialNone(), false, true, false);
	BlockID SOUL_SOIL = RegisterBlock("soul_soil", new MaterialNone(), false, true, false);
	BlockID BONE_BLOCK = RegisterBlock("bone_block", new MaterialNone(), false, true, false);
	BlockID BLACKSTONE = RegisterBlock("blackstone", new MaterialNone(), false, true, false);
	BlockID BASALT = RegisterBlock("basalt", new MaterialNone(), false, true, false);
	BlockID SMOOTH_BASALT = RegisterBlock("smooth_basalt", new MaterialNone(), false, true, false);
	BlockID END_STONE = RegisterBlock("end_stone", new MaterialNone(), false, true, false);
	BlockID COAL_ORE = RegisterBlock("coal_ore", new MaterialNone(), false, true, false);
	BlockID DEEPSLATE_COAL_ORE = RegisterBlock("deepslate_coal_ore", new MaterialNone(), false, true, false);
	BlockID IRON_ORE = RegisterBlock("iron_ore", new MaterialNone(), false, true, false);
	BlockID DEEPSLATE_IRON_ORE = RegisterBlock("deepslate_iron_ore", new MaterialNone(), false, true, false);
	BlockID COPPER_ORE = RegisterBlock("copper_ore", new MaterialNone(), false, true, false);
	BlockID DEEPSLATE_COPPER_ORE = RegisterBlock("deepslate_copper_ore", new MaterialNone(), false, true, false);
	BlockID GOLD_ORE = RegisterBlock("gold_ore", new MaterialNone(), false, true, false);
	BlockID DEEPSLATE_GOLD_ORE = RegisterBlock("deepslate_gold_ore", new MaterialNone(), false, true, false);
	BlockID REDSTONE_ORE = RegisterBlock("redstone_ore", new MaterialNone(), false, true, false);
	BlockID DEEPSLATE_REDSTONE_ORE = RegisterBlock("deepslate_redstone_ore", new MaterialNone(), false, true, false);
	BlockID EMERALD_ORE = RegisterBlock("emerald_ore", new MaterialNone(), false, true, false);
	BlockID DEEPSLATE_EMERALD_ORE = RegisterBlock("deepslate_emerald_ore", new MaterialNone(), false, true, false);
	BlockID LAPIS_ORE = RegisterBlock("lapis_ore", new MaterialNone(), false, true, false);
	BlockID DEEPSLATE_LAPIS_ORE = RegisterBlock("deepslate_lapis_ore", new MaterialNone(), false, true, false);
	BlockID DIAMOND_ORE = RegisterBlock("diamond_ore", new MaterialNone(), false, true, false);
	BlockID DEEPSLATE_DIAMOND_ORE = RegisterBlock("deepslate_diamond_ore", new MaterialNone(), false, true, false);
	BlockID NETHER_GOLD_ORE = RegisterBlock("nether_gold_ore", new MaterialNone(), false, true, false);
	BlockID NETHER_QUARTZ_ORE = RegisterBlock("nether_quartz_ore", new MaterialNone(), false, true, false);
	BlockID ANCIENT_DEBRIS = RegisterBlock("ancient_debris", new MaterialNone(), false, true, false);
	BlockID RAW_IRON_BLOCK = RegisterBlock("raw_iron_block", new MaterialNone(), false, true, false);
	BlockID RAW_COPPER_BLOCK = RegisterBlock("raw_copper_block", new MaterialNone(), false, true, false);
	BlockID RAW_GOLD_BLOCK = RegisterBlock("raw_gold_block", new MaterialNone(), false, true, false);
	BlockID GLOWSTONE = RegisterBlock("glowstone", new MaterialNone(), false, true, false);
	BlockID AMETHYST_BLOCK = RegisterBlock("amethyst_block", new MaterialNone(), false, true, false);
	BlockID BUDDING_AMETHYST = RegisterBlock("budding_amethyst", new MaterialNone(), false, true, false);
	BlockID SPRUCE_LOG = RegisterBlock("spruce_log", new MaterialNone(), false, true, false);
	BlockID STRIPPED_SPRUCE_LOG = RegisterBlock("stripped_spruce_log", new MaterialNone(), false, true, false);
	BlockID BIRCH_LOG = RegisterBlock("birch_log", new MaterialNone(), false, true, false);
	BlockID STRIPPED_BIRCH_LOG = RegisterBlock("stripped_birch_log", new MaterialNone(), false, true, false);
	BlockID JUNGLE_LOG = RegisterBlock("jungle_log", new MaterialNone(), false, true, false);
	BlockID STRIPPED_JUNGLE_LOG = RegisterBlock("stripped_jungle_log", new MaterialNone(), false, true, false);
	BlockID ACACIA_LOG = RegisterBlock("acacia_log", new MaterialNone(), false, true, false);
	BlockID STRIPPED_ACACIA_LOG = RegisterBlock("stripped_acacia_log", new MaterialNone(), false, true, false);
	BlockID DARK_OAK_LOG = RegisterBlock("dark_oak_log", new MaterialNone(), false, true, false);
	BlockID STRIPPED_DARK_OAK_LOG = RegisterBlock("stripped_dark_oak_log", new MaterialNone(), false, true, false);
	BlockID CHERRY_LOG = RegisterBlock("cherry_log", new MaterialNone(), false, true, false);
	BlockID STRIPPED_CHERRY_LOG = RegisterBlock("stripped_cherry_log", new MaterialNone(), false, true, false);
	BlockID MANGROVE_LOG = RegisterBlock("mangrove_log", new MaterialNone(), false, true, false);
	BlockID STRIPPED_MANGROVE_LOG = RegisterBlock("stripped_mangrove_log", new MaterialNone(), false, true, false);
	BlockID CRIMSON_STEM = RegisterBlock("crimson_stem", new MaterialNone(), false, true, false);
	BlockID STRIPPED_CRIMSON_STEM = RegisterBlock("stripped_crimson_stem", new MaterialNone(), false, true, false);
	BlockID WARPED_STEM = RegisterBlock("warped_stem", new MaterialNone(), false, true, false);
	BlockID STRIPPED_WARPED_STEM = RegisterBlock("stripped_warped_stem", new MaterialNone(), false, true, false);
	BlockID MANGROVE_ROOTS = RegisterBlock("mangrove_roots", new MaterialNone(), true, true, false);
	BlockID MUDDY_MANGROVE_ROOTS = RegisterBlock("muddy_mangrove_roots", new MaterialNone(), false, true, false);
	BlockID MUSHROOM_STEM = RegisterBlock("mushroom_stem", new MaterialNone(), false, true, false);
	BlockID BROWN_MUSHROOM_BLOCK = RegisterBlock("brown_mushroom_block", new MaterialNone(), false, true, false);
	BlockID RED_MUSHROOM_BLOCK = RegisterBlock("red_mushroom_block", new MaterialNone(), false, true, false);
	BlockID NETHER_WART_BLOCK = RegisterBlock("nether_wart_block", new MaterialNone(), false, true, false);
	BlockID WARPED_WART_BLOCK = RegisterBlock("warped_wart_block", new MaterialNone(), false, true, false);
	BlockID SHROOMLIGHT = RegisterBlock("shroomlight", new MaterialNone(), false, true, false);
	BlockID DRIED_KELP_BLOCK = RegisterBlock("dried_kelp_block", new MaterialNone(), false, true, false);
	BlockID TUBE_CORAL_BLOCK = RegisterBlock("tube_coral_block", new MaterialNone(), false, true, false);
	BlockID BRAIN_CORAL_BLOCK = RegisterBlock("brain_coral_block", new MaterialNone(), false, true, false);
	BlockID BUBBLE_CORAL_BLOCK = RegisterBlock("bubble_coral_block", new MaterialNone(), false, true, false);
	BlockID FIRE_CORAL_BLOCK = RegisterBlock("fire_coral_block", new MaterialNone(), false, true, false);
	BlockID HORN_CORAL_BLOCK = RegisterBlock("horn_coral_block", new MaterialNone(), false, true, false);
	BlockID DEAD_TUBE_CORAL_BLOCK = RegisterBlock("dead_tube_coral_block", new MaterialNone(), false, true, false);
	BlockID DEAD_BRAIN_CORAL_BLOCK = RegisterBlock("dead_brain_coral_block", new MaterialNone(), false, true, false);
	BlockID DEAD_BUBBLE_CORAL_BLOCK = RegisterBlock("dead_bubble_coral_block", new MaterialNone(), false, true, false);
	BlockID DEAD_FIRE_CORAL_BLOCK = RegisterBlock("dead_fire_coral_block", new MaterialNone(), false, true, false);
	BlockID DEAD_HORN_CORAL_BLOCK = RegisterBlock("dead_horn_coral_block", new MaterialNone(), false, true, false);
	BlockID SPONGE = RegisterBlock("sponge", new MaterialNone(), false, true, false);
	BlockID WET_SPONGE = RegisterBlock("wet_sponge", new MaterialNone(), false, true, false);
	BlockID MELON = RegisterBlock("melon", new MaterialNone(), false, true, false);
	BlockID PUMPKIN = RegisterBlock("pumpkin", new MaterialNone(), false, true, false);
	BlockID CARVED_PUMPKIN = RegisterBlock("carved_pumpkin", new MaterialNone(), false, true, false);
	BlockID JACK_O_LANTERN = RegisterBlock("jack_o_lantern", new MaterialNone(), false, true, false);
	BlockID HAY_BALE = RegisterBlock("hay_block", new MaterialNone(), false, true, false);
	BlockID BEE_NEST = RegisterBlock("bee_nest", new MaterialNone(), false, true, false);
	BlockID HONEYCOMB_BLOCK = RegisterBlock("honeycomb_block", new MaterialNone(), false, true, false);
	BlockID OCHRE_FROGLIGHT = RegisterBlock("ochre_froglight", new MaterialNone(), false, true, false);
	BlockID VERDANT_FROGLIGHT = RegisterBlock("verdant_froglight", new MaterialNone(), false, true, false);
	BlockID PEARLESCENT_FROGLIGHT = RegisterBlock("pearlescent_froglight", new MaterialNone(), false, true, false);
	BlockID SCULK = RegisterBlock("sculk", new MaterialNone(), false, true, false);
	BlockID SCULK_CATALYST = RegisterBlock("sculk_catalyst", new MaterialNone(), false, true, false);
	BlockID BEDROCK = RegisterBlock("bedrock", new MaterialNone(), false, true, false);
	BlockID SPRUCE_LEAVES = RegisterBlock("spruce_leaves", new MaterialNone(), true, true, false);
	BlockID BIRCH_LEAVES = RegisterBlock("birch_leaves", new MaterialNone(), true, true, false);
	BlockID JUNGLE_LEAVES = RegisterBlock("jungle_leaves", new MaterialNone(), true, true, false);
	BlockID ACACIA_LEAVES = RegisterBlock("acacia_leaves", new MaterialNone(), true, true, false);
	BlockID DARK_OAK_LEAVES = RegisterBlock("dark_oak_leaves", new MaterialNone(), true, true, false);
	BlockID MANGROVE_LEAVES = RegisterBlock("mangrove_leaves", new MaterialNone(), true, true, false);
	BlockID CHERRY_LEAVES = RegisterBlock("cherry_leaves", new MaterialNone(), true, true, false);
	BlockID AZALEA_LEAVES = RegisterBlock("azalea_leaves", new MaterialNone(), true, true, false);
	BlockID FLOWERING_AZALEA_LEAVES = RegisterBlock("flowering_azalea_leaves", new MaterialNone(), true, true, false);
	BlockID OAK_LEAF = RegisterBlock("oak_leaves", new MaterialNone(), true, true, false);
	BlockID SEA_LANTERN = RegisterBlock("sea_lantern", new MaterialNone(), false, true, false);
	BlockID REDSTONE_LAMP = RegisterBlock("redstone_lamp", new MaterialNone(), false, true, false);
	BlockID CRAFTING_TABLE = RegisterBlock("crafting_table", new MaterialNone(), false, true, false);
	BlockID COPPER_BULB = RegisterBlock("copper_bulb", new MaterialNone(), false, true, false);
	BlockID EXPOSED_COPPER_BULB = RegisterBlock("exposed_copper_bulb", new MaterialNone(), false, true, false);
	BlockID OXIDIZED_COPPER_BULB = RegisterBlock("oxidized_copper_bulb", new MaterialNone(), false, true, false);
	BlockID WEATHERED_COPPER_BULB = RegisterBlock("weathered_copper_bulb", new MaterialNone(), false, true, false);
	BlockID COPPER_GRATE = RegisterBlock("copper_grate", new MaterialNone(), true, true, false);
	BlockID EXPOSED_COPPER_GRATE = RegisterBlock("exposed_copper_grate", new MaterialNone(), true, true, false);
	BlockID OXIDIZED_COPPER_GRATE = RegisterBlock("oxidized_copper_grate", new MaterialNone(), true, true, false);
	BlockID WEATHERED_COPPER_GRATE = RegisterBlock("weathered_copper_grate", new MaterialNone(), true, true, false);
	BlockID CUT_COPPER = RegisterBlock("cut_copper", new MaterialNone(), false, true, false);
	BlockID EXPOSED_CUT_COPPER = RegisterBlock("exposed_cut_copper", new MaterialNone(), false, true, false);
	BlockID OXIDIZED_CUT_COPPER = RegisterBlock("oxidized_cut_copper", new MaterialNone(), false, true, false);
	BlockID WEATHERED_CUT_COPPER = RegisterBlock("weathered_cut_copper", new MaterialNone(), false, true, false);
	BlockID CHISELED_COPPER = RegisterBlock("chiseled_copper", new MaterialNone(), false, true, false);
	BlockID EXPOSED_CHISELED_COPPER = RegisterBlock("exposed_chiseled_copper", new MaterialNone(), false, true, false);
	BlockID OXIDIZED_CHISELED_COPPER = RegisterBlock("oxidized_chiseled_copper", new MaterialNone(), false, true, false);
	BlockID WEATHERED_CHISELED_COPPER = RegisterBlock("weathered_chiseled_copper", new MaterialNone(), false, true, false);
	BlockID WARPED_HYPHAE = RegisterBlock("warped_hyphae", new MaterialNone(), false, true, false);
	BlockID STRIPPED_WARPED_HYPHAE = RegisterBlock("stripped_warped_hyphae", new MaterialNone(), false, true, false);
	BlockID CRIMSON_HYPHAE = RegisterBlock("crimson_hyphae", new MaterialNone(), false, true, false);
	BlockID STRIPPED_CRIMSON_HYPHAE = RegisterBlock("stripped_crimson_hyphae", new MaterialNone(), false, true, false);
	BlockID MOSSY_COBBLESTONE = RegisterBlock("mossy_cobblestone", new MaterialNone(), false, true, false);
	BlockID DEEPSLATE = RegisterBlock("deepslate", new MaterialNone(), false, true, false);
	BlockID COBBLED_DEEPSLATE = RegisterBlock("cobbled_deepslate", new MaterialNone(), false, true, false);
	BlockID CHISELED_DEEPSLATE = RegisterBlock("chiseled_deepslate", new MaterialNone(), false, true, false);
	BlockID POLISHED_DEEPSLATE = RegisterBlock("polished_deepslate", new MaterialNone(), false, true, false);
	BlockID DEEPSLATE_TILES = RegisterBlock("deepslate_tiles", new MaterialNone(), false, true, false);
	BlockID CRACKED_DEEPSLATE_TILES = RegisterBlock("deepslate_tiles", new MaterialNone(), false, true, false);
	BlockID REINFORCED_DEEPSLATE = RegisterBlock("reinforced_deepslate", new MaterialNone(), false, true, false);
	BlockID TUFF = RegisterBlock("tuff", new MaterialNone(), false, true, false);
	BlockID CHISELED_TUFF = RegisterBlock("chiseled_tuff", new MaterialNone(), false, true, false);
	BlockID POLISHED_TUFF = RegisterBlock("polished_tuff", new MaterialNone(), false, true, false);
	BlockID PACKED_MUD = RegisterBlock("packed_mud", new MaterialNone(), false, true, false);
	BlockID SANDSTONE = RegisterBlock("sandstone", new MaterialNone(), false, true, false);
	BlockID CHISELED_SANDSTONE = RegisterBlock("chiseled_sandstone", new MaterialNone(), false, true, false);
	BlockID SMOOTH_SANDSTONE = RegisterBlock("smooth_sandstone", new MaterialNone(), false, true, false);
	BlockID CUT_SANDSTONE = RegisterBlock("cut_sandstone", new MaterialNone(), false, true, false);
	BlockID RED_SANDSTONE = RegisterBlock("red_sandstone", new MaterialNone(), false, true, false);
	BlockID CHISELED_RED_SANDSTONE = RegisterBlock("chiseled_red_sandstone", new MaterialNone(), false, true, false);
	BlockID SMOOTH_RED_SANDSTONE = RegisterBlock("smooth_red_sandstone", new MaterialNone(), false, true, false);
	BlockID CUT_RED_SANDSTONE = RegisterBlock("cut_red_sandstone", new MaterialNone(), false, true, false);
	BlockID DARK_PRISMARINE = RegisterBlock("dark_prismarine", new MaterialNone(), false, true, false);
	BlockID PURPUR_BLOCK = RegisterBlock("purpur_block", new MaterialNone(), false, true, false);
	BlockID PURPUR_PILLAR = RegisterBlock("purpur_pillar", new MaterialNone(), false, true, false);
	BlockID COAL_BLOCK = RegisterBlock("coal_block", new MaterialNone(), false, true, false);
	BlockID IRON_BLOCK = RegisterBlock("iron_block", new MaterialNone(), false, true, false);
	BlockID GOLD_BLOCK = RegisterBlock("gold_block", new MaterialNone(), false, true, false);
	BlockID REDSTONE_BLOCK = RegisterBlock("redstone_block", new MaterialNone(), false, true, false);
	BlockID EMERALD_BLOCK = RegisterBlock("emerald_block", new MaterialNone(), false, true, false);
	BlockID LAPIS_BLOCK = RegisterBlock("lapis_block", new MaterialNone(), false, true, false);
	BlockID DIAMOND_BLOCK = RegisterBlock("diamond_block", new MaterialNone(), false, true, false);
	BlockID NETHERITE_BLOCK = RegisterBlock("netherite_block", new MaterialNone(), false, true, false);
	BlockID QUARTZ_BLOCK = RegisterBlock("quartz_block", new MaterialNone(), false, true, false);
	BlockID CHISELED_QUARTZ_BLOCK = RegisterBlock("chiseled_quartz_block", new MaterialNone(), false, true, false);
	BlockID QUARTZ_PILLAR = RegisterBlock("quartz_pillar", new MaterialNone(), false, true, false);
	BlockID SMOOTH_QUARTZ = RegisterBlock("smooth_quartz", new MaterialNone(), false, true, false);

	BlockID ANVIL = RegisterBlock("anvil", new MaterialNone(), false, true, false);

	void Initialize() {
		BlockTextureArray.Gen();
		BlockTextureArray.height = 16;
		BlockTextureArray.width = 16;
		BlockTextureAtlas.Gen();
		BlockTextureAtlas.SetSize(16 * 512, 16 * 512);

		InitializeBlockModels();

		BlockTextureArray.UploadToGPU();
		BlockTextureAtlas.UploadToGPU();
	}

	~BlockList() {
		CleanUp();
	}

	void CleanUp() {
		for (const auto& obj : BlockTypeData) {
			delete obj;
		}

		BlockTypeData.clear();
	}

	Block* getBlockType(BlockID id) {
		return BlockTypeData[id];
	}

	BlockID RegisterBlock(std::string BlockName, Material* material, bool transparency, bool solid, bool isFluid);

} ;

extern BlockList Blocks;
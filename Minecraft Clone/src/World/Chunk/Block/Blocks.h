#pragma once
#ifndef BLOCKS_H
#define BLOCKS_H
#include <unordered_map>
#include "Block.h"
#include "Material/BlockMaterial.h"
#include "Texture/BlockTexture.h"

class BlockList{
private:
	std::unordered_map<std::string, BlockID> BlockIDNameData;
	int BlockTypeCount = 0;

	void InitializeTextures();
public:
	std::unordered_map<unsigned int, Block*> BlockTypeData;

	TextureArray BlockTextureArray;

	BlockID AIR = RegisterBlock("air", new MaterialNone(), true, false, false);
	BlockID STONE = RegisterBlock("stone", new MaterialNone(), false, true, false);
	BlockID DIRT = RegisterBlock("dirt", new MaterialDirt(), false, true, false);
	BlockID WATER = RegisterBlock("water_still", new MaterialFluid(5), true, false, true);
	BlockID LAVA = RegisterBlock("lava", new MaterialFluid(5), true, false, true);
	BlockID GRASS = RegisterBlock("grass_block", new MaterialGrass(0.1, 0.1), false, true, false);
	BlockID SAND = RegisterBlock("sand", new MaterialGravity(), false, true, false);
	BlockID OAK_LOG = RegisterBlock("oak_log", new MaterialNone(), false, true, false);
	BlockID OAK_LEAF = RegisterBlock("oak_leaf", new MaterialNone(), true, true, false);
	BlockID DIAMOND_BLOCK = RegisterBlock("diamond_block", new MaterialNone(), false, true, false);
	BlockID OAK_PLANK = RegisterBlock("oak_plank", new MaterialNone(), false, true, false);
	BlockID COBBLESTONE = RegisterBlock("cobblestone", new MaterialNone(), false, true, false);
	BlockID BRICK = RegisterBlock("brick", new MaterialNone(), false, true, false);
	BlockID WHITE_CONCRETE = RegisterBlock("white_concrete", new MaterialNone(), false, true, false);
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

	BlockID NULL_BLOCK = RegisterBlock("null_block", new MaterialNone(), false, false, false);

	void Initialize() {
		BlockTextureArray.Gen();
		BlockTextureArray.height = 16;
		BlockTextureArray.width = 16;

		InitializeTextures();

		BlockTextureArray.UploadToGPU();
	}

	~BlockList() {
		CleanUp();
	}

	void CleanUp() {
		for (const auto& obj : BlockTypeData) {
			delete obj.second;
		}

		BlockTypeData.clear();
	}

	Block* getBlockType(BlockID id) {
		return BlockTypeData[id];
	}

	BlockID RegisterBlock(std::string BlockName, Material* material, bool transparency, bool solid, bool isFluid);

} ;

extern BlockList Blocks;

#endif
#pragma once

#include <unordered_map>
#include "Block.h"
#include "BlockTexture.h"

std::unordered_map<unsigned int, BlockType*> Blocks;

struct Block {
	BlockID ID = NULL;

	BlockType* Get() {
		return Blocks[ID];
	}

	BlockTexture* Texture = nullptr;
	
};

Block RegisterNewBlock(bool transparency, bool solid, bool isFluid) {
	BlockID ID = Blocks.size();
	BlockType* NewBlock = new BlockType(transparency, solid, isFluid);
	Blocks[ID] = NewBlock;

	Block block;
	block.ID = ID;

	block.Texture = new BlockTexture;

	return block;
}

Block AIR = RegisterNewBlock(true, false, false);
Block STONE = RegisterNewBlock(false, true, false);
Block DIRT = RegisterNewBlock(false, true, false);
Block WATER = RegisterNewBlock(true, false, true);
Block GRASS = RegisterNewBlock(false, true, false);
Block SAND = RegisterNewBlock(false, true, false);
Block OAK_LOG = RegisterNewBlock(false, true, false);
Block OAK_LEAF = RegisterNewBlock(true, true, false);

void InitializeBlockTextures() {
	BlockTextureArray.height = 16;
	BlockTextureArray.width = 16;

	STONE.Texture->SetFacesCustom("assets/textures/array/block/stone.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
	DIRT.Texture->SetFacesCustom("assets/textures/array/block/dirt.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
	WATER.Texture->SetFacesCustom("assets/textures/array/block/water_still.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);

	GRASS.Texture->SetFaceTop("assets/textures/array/block/grass_block_top.png");
	GRASS.Texture->SetFaceBottom("assets/textures/array/block/dirt.png");
	GRASS.Texture->SetFacesCustom("assets/textures/array/block/grass_block_side.png",LEFT,RIGHT,FRONT,BACK);

	SAND.Texture->SetFacesCustom("assets/textures/array/block/sand.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);

	GRASS.Texture->SetFacesCustom("assets/textures/array/block/oak_log_top.png", TOP, BOTTOM);
	GRASS.Texture->SetFacesCustom("assets/textures/array/block/oak_log.png", LEFT, RIGHT, FRONT, BACK);
}
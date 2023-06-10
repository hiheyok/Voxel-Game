#pragma once
#ifndef BLOCKS_H
#define BLOCKS_H
#include <unordered_map>
#include "Block.h"
#include "BlockTexture.h"

struct Block {

	~Block() {
		delete block;
		delete Texture;
	}

	BlockID ID = NULL;

	BlockType* block = nullptr;

	BlockTexture* Texture = nullptr;

};

__declspec(selectany) std::unordered_map<unsigned int, Block*> BlockList;



inline BlockID RegisterNewBlock(bool transparency, bool solid, bool isFluid) {
	BlockID ID = (BlockID)BlockList.size();
	BlockType* NewBlock = new BlockType(transparency, solid, isFluid);

	Block* block = new Block;
	block->ID = ID;
	block->block = NewBlock;
	block->Texture = new BlockTexture;

	BlockList[ID] = block;

	return ID;
}

__declspec(selectany) BlockID AIR = RegisterNewBlock(true, false, false);
__declspec(selectany) BlockID STONE = RegisterNewBlock(false, true, false);
__declspec(selectany) BlockID DIRT = RegisterNewBlock(false, true, false);
__declspec(selectany) BlockID WATER = RegisterNewBlock(false, true, false);
__declspec(selectany) BlockID GRASS = RegisterNewBlock(false, true, false);
__declspec(selectany) BlockID SAND = RegisterNewBlock(false, true, false);
__declspec(selectany) BlockID OAK_LOG = RegisterNewBlock(false, true, false);
__declspec(selectany) BlockID OAK_LEAF = RegisterNewBlock(false, true, false);
__declspec(selectany) BlockID DIAMOND_BLOCK = RegisterNewBlock(false, true, false);

__declspec(selectany) BlockID OAK_PLANK = RegisterNewBlock(false, true, false);

__declspec(selectany) BlockID COBBLESTONE = RegisterNewBlock(false, true, false);

__declspec(selectany) BlockID BRICK = RegisterNewBlock(false, true, false);

__declspec(selectany) BlockID NULL_BLOCK = RegisterNewBlock(false, false, false);

inline void InitializeBlockTextures() {
	BlockTextureArray.Gen();
	BlockTextureArray.height = 16;
	BlockTextureArray.width = 16;

	BlockList[STONE]->Texture->SetFacesCustom("assets/textures/array/block/stone.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
	BlockList[DIRT]->Texture->SetFacesCustom("assets/textures/array/block/dirt.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
	BlockList[WATER]->Texture->SetFacesCustom("assets/textures/array/block/water_still.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);

	BlockList[GRASS]->Texture->SetFaceTop("assets/textures/array/block/grass_block_top.png");
	BlockList[GRASS]->Texture->SetFaceBottom("assets/textures/array/block/dirt.png");
	BlockList[GRASS]->Texture->SetFacesCustom("assets/textures/array/block/grass_block_side.png",LEFT,RIGHT,FRONT,BACK);

	BlockList[SAND]->Texture->SetFacesCustom("assets/textures/array/block/sand.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);

	BlockList[OAK_LOG]->Texture->SetFacesCustom("assets/textures/array/block/oak_log_top.png", TOP, BOTTOM);
	BlockList[OAK_LOG]->Texture->SetFacesCustom("assets/textures/array/block/oak_log.png", LEFT, RIGHT, FRONT, BACK);

	BlockList[OAK_LEAF]->Texture->SetFacesCustom("assets/textures/array/block/oak_leaves.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
	BlockList[DIAMOND_BLOCK]->Texture->SetFacesCustom("assets/textures/array/block/diamond_block.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);

	BlockList[COBBLESTONE]->Texture->SetFacesCustom("assets/textures/array/block/cobblestone.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
	BlockList[OAK_PLANK]->Texture->SetFacesCustom("assets/textures/array/block/oak_planks.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
	
	BlockList[BRICK]->Texture->SetFacesCustom("assets/textures/array/block/bricks.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);

	BlockTextureArray.UploadToGPU();
}

#endif
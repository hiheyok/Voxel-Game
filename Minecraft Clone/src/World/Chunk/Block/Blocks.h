#pragma once
#ifndef BLOCKS_H
#define BLOCKS_H
#include <unordered_map>
#include "Block.h"
#include "BlockTexture.h"
struct Block {

	~Block() {
		delete Properties;
		delete Texture;
	}

	BlockID ID = NULL;

	BlockType* Properties = nullptr;

	BlockTexture* Texture = nullptr;
};

//__declspec(selectany) 



class BlockList {
private:
	std::unordered_map<unsigned int, Block*> BlockTypeData;
public:
	TextureArray BlockTextureArray;

	BlockID AIR = RegisterNewBlock(true, false, false);
	BlockID STONE = RegisterNewBlock(false, true, false);
	BlockID DIRT = RegisterNewBlock(false, true, false);
	BlockID WATER = RegisterNewBlock(true, false, true);
	BlockID GRASS = RegisterNewBlock(false, true, false);
	BlockID SAND = RegisterNewBlock(false, true, false);
	BlockID OAK_LOG = RegisterNewBlock(false, true, false);
	BlockID OAK_LEAF = RegisterNewBlock(true, true, false);
	BlockID DIAMOND_BLOCK = RegisterNewBlock(false, true, false);
	BlockID OAK_PLANK = RegisterNewBlock(false, true, false);
	BlockID COBBLESTONE = RegisterNewBlock(false, true, false);
	BlockID BRICK = RegisterNewBlock(false, true, false);
	BlockID WHITE_CONCRETE = RegisterNewBlock(false, true, false);

	BlockID NULL_BLOCK = RegisterNewBlock(false, false, false);

	void RegisterBlock() {

	}

	void Initialize() {
		BlockTextureArray.Gen();
		BlockTextureArray.height = 16;
		BlockTextureArray.width = 16;

		SetFacesCustom(STONE, "assets/textures/array/block/stone.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
		SetFacesCustom(DIRT, "assets/textures/array/block/dirt.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
		SetFacesCustom(WATER, "assets/textures/array/block/water_still.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);

		SetFaceTop(GRASS, "assets/textures/array/block/grass_block_top.png");
		SetFaceBottom(GRASS, "assets/textures/array/block/dirt.png");
		SetFacesCustom(GRASS, "assets/textures/array/block/grass_block_side.png", LEFT, RIGHT, FRONT, BACK);

		SetFacesCustom(SAND, "assets/textures/array/block/sand.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);

		SetFacesCustom(OAK_LOG, "assets/textures/array/block/oak_log_top.png", TOP, BOTTOM);
		SetFacesCustom(OAK_LOG, "assets/textures/array/block/oak_log.png", LEFT, RIGHT, FRONT, BACK);

		SetFacesCustom(OAK_LEAF, "assets/textures/array/block/oak_leaves.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
		SetFacesCustom(DIAMOND_BLOCK, "assets/textures/array/block/diamond_block.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);

		SetFacesCustom(COBBLESTONE, "assets/textures/array/block/cobblestone.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
		SetFacesCustom(OAK_PLANK, "assets/textures/array/block/oak_planks.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);

		SetFacesCustom(BRICK, "assets/textures/array/block/bricks.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);
		SetFacesCustom(WHITE_CONCRETE, "assets/textures/array/block/white_concrete.png", FRONT, BACK, LEFT, RIGHT, BOTTOM, TOP);

		BlockTextureArray.UploadToGPU();
	}

	~BlockList() {

		for (const auto& obj : BlockTypeData) {
			delete obj.second;
		}

		BlockTypeData.clear();
	}

	Block* getBlockType(BlockID id) {
		return BlockTypeData[id];
	}

	inline BlockID RegisterNewBlock(bool transparency, bool solid, bool isFluid) {
		BlockID ID = (BlockID)BlockTypeData.size();
		BlockType* NewBlock = new BlockType(transparency, solid, isFluid);

		Block* block = new Block;
		block->ID = ID;
		block->Properties = NewBlock;
		block->Texture = new BlockTexture;

		BlockTypeData[ID] = block;
		Logger.LogInfo("Register","Registered new block (ID): " + std::to_string(ID));
		return ID;
	}

	int SetFaceFront(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceFront(id);
	}
	int SetFaceBack(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceBack(id);
	}
	int SetFaceWest(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceWest(id);
	}
	int SetFaceEast(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceEast(id);
	}
	int SetFaceTop(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceTop(id);
	}
	int SetFaceBottom(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceBottom(id);
	}
	int SetFacesCustom(BlockID id, std::string file, int side0 = 0xFF, int side1 = 0xFF, int side2 = 0xFF, int side3 = 0xFF, int side4 = 0xFF, int side5 = 0xFF) {
		BlockTextureArray.AddTextureToArray(file);

		int TexID = BlockTextureArray.GetLayers();

		Block* b = BlockTypeData[id];
		
		b->Texture->SetFacesCustom(TexID, side0, side1, side2 , side3, side4, side5);

	}

} __declspec(selectany)  Blocks;

#endif
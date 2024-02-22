#pragma once
#ifndef BLOCKS_H
#define BLOCKS_H
#include <unordered_map>
#include "Block.h"
#include "Material/BlockMaterial.h"
#include "Texture/BlockTexture.h"

struct BlockRegistration {
	Material* material = nullptr;
	bool transparency = false;
	bool solid = false;
	bool isFluid = false;
	std::string BlockName = "";

	BlockID blockID = 0;
};

class BlockList {
private:
	

	std::deque<BlockRegistration> RegistrationQueue;
	std::unordered_map<std::string, BlockID> BlockIDNameData;
	int BlockTypeCount = 0;

	void InitializeTextures();
public:
	std::unordered_map<unsigned int, Block*> BlockTypeData;

	TextureArray BlockTextureArray;

	BlockID AIR = QueueRegister("air", new MaterialNone(), true, false, false);
	BlockID STONE = QueueRegister("stone", new MaterialNone(), false, true, false);
	BlockID DIRT = QueueRegister("dirt", new MaterialDirt(), false, true, false);
	BlockID WATER = QueueRegister("water_still", new MaterialFluid(5), true, false, true);
	BlockID LAVA = QueueRegister("lava", new MaterialFluid(5), true, false, true);
	BlockID GRASS = QueueRegister("grass_block", new MaterialGrass(0.1, 0.1), false, true, false);
	BlockID SAND = QueueRegister("sand", new MaterialNone(), false, true, false);
	BlockID OAK_LOG = QueueRegister("oak_log", new MaterialNone(), false, true, false);
	BlockID OAK_LEAF = QueueRegister("oak_leaf", new MaterialNone(), true, true, false);
	BlockID DIAMOND_BLOCK = QueueRegister("diamond_block", new MaterialNone(), false, true, false);
	BlockID OAK_PLANK = QueueRegister("oak_plank", new MaterialNone(), false, true, false);
	BlockID COBBLESTONE = QueueRegister("cobblestone", new MaterialNone(), false, true, false);
	BlockID BRICK = QueueRegister("brick", new MaterialNone(), false, true, false);
	BlockID WHITE_CONCRETE = QueueRegister("white_concrete", new MaterialNone(), false, true, false);


	BlockID BLUE_STAINED_GLASS = QueueRegister("blue_stained_glass", new MaterialNone(), true, true, false);
	BlockID GREEN_STAINED_GLASS = QueueRegister("green_stained_glass", new MaterialNone(), true, true, false);
	BlockID BLACK_STAINED_GLASS = QueueRegister("black_stained_glass", new MaterialNone(), true, true, false);
	BlockID BROWN_STAINED_GLASS = QueueRegister("brown_stained_glass", new MaterialNone(), true, true, false);
	BlockID CYAN_STAINED_GLASS = QueueRegister("cyan_stained_glass", new MaterialNone(), true, true, false);
	BlockID GRAY_STAINED_GLASS = QueueRegister("gray_stained_glass", new MaterialNone(), true, true, false);
	BlockID LIGHT_BLUE_STAINED_GLASS = QueueRegister("light_blue_stained_glass", new MaterialNone(), true, true, false);
	BlockID LIGHT_GRAY_STAINED_GLASS = QueueRegister("light_gray_stained_glass", new MaterialNone(), true, true, false);
	BlockID LIME_STAINED_GLASS = QueueRegister("lime_stained_glass", new MaterialNone(), true, true, false);
	BlockID MAGENTA_STAINED_GLASS = QueueRegister("magenta_stained_glass", new MaterialNone(), true, true, false);
	BlockID ORANGE_STAINED_GLASS = QueueRegister("orange_stained_glass", new MaterialNone(), true, true, false);
	BlockID PINK_STAINED_GLASS = QueueRegister("pink_stained_glass", new MaterialNone(), true, true, false);
	BlockID PURPLE_STAINED_GLASS = QueueRegister("purple_stained_glass", new MaterialNone(), true, true, false);
	BlockID RED_STAINED_GLASS = QueueRegister("red_stained_glass", new MaterialNone(), true, true, false);
	BlockID WHITE_STAINED_GLASS = QueueRegister("white_stained_glass", new MaterialNone(), true, true, false);
	BlockID YELLOW_STAINED_GLASS = QueueRegister("yellow_stained_glass", new MaterialNone(), true, true, false);

	BlockID NULL_BLOCK = QueueRegister("null_block", new MaterialNone(), false, false, false);

	void RegisterAll();

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

	void RegisterNewBlock(BlockRegistration reg);

	BlockID QueueRegister(std::string BlockName, Material* material, bool transparency, bool solid, bool isFluid);

	void SetFaceFront(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceFront(id);
	}
	void SetFaceBack(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceBack(id);
	}
	void SetFaceWest(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceWest(id);
	}
	void SetFaceEast(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceEast(id);
	}
	void SetFaceTop(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceTop(id);
	}
	void SetFaceBottom(BlockID id, std::string file) {
		BlockTextureArray.AddTextureToArray(file);
		BlockTypeData[id]->Texture->SetFaceBottom(id);
	}
	void SetFacesCustom(BlockID id, std::string file, int side0 = 0xFF, int side1 = 0xFF, int side2 = 0xFF, int side3 = 0xFF, int side4 = 0xFF, int side5 = 0xFF) {
		BlockTextureArray.AddTextureToArray(file);

		int TexID = BlockTextureArray.GetLayers();

		Block* b = BlockTypeData[id];
		
		b->Texture->SetFacesCustom(TexID, side0, side1, side2 , side3, side4, side5);
	}

} __declspec(selectany)  Blocks;

#endif
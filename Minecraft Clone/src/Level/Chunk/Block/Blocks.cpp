#include "Blocks.h"
#include "Type/BlockTypes.h"
#include <nlohmann/json.hpp>
#include "../../../FileManager/Files.h"
#include <fstream>
#include "../../../RenderEngine/BlockModel/ModelLoader.h"
#include "../../../Utils/Containers/skaHashmap.h"
#include <iostream>
#include <sys/types.h>
#include <filesystem>
using namespace std;

using json = nlohmann::json;

BlockID BlockList::RegisterBlock(std::string BlockName, Material* material, bool transparency, bool solid, bool isFluid) {
	BlockID ID = static_cast<BlockID>(BlockTypeData.size());
	BlockType* NewBlock = new BlockType(transparency, solid, isFluid);

	Block* block = material->BuildNewBlockType();

	// MaterialType Type = material->type;

	block->ID = ID;
	block->Properties = NewBlock;
	block->Texture = new BlockTexture;
	block->BlockName = BlockName;

	BlockTypeData.emplace_back(block);

	BlockIDNameData[BlockName] = ID;

	Logger.LogInfo("Register", "Registered new block (ID): " + std::to_string(ID));


	return ID;
}

void BlockList::AddAssets(std::string namespaceIn) {
	try {
		std::vector<std::string> allOtherBlocks{};
		std::string path = "assets/"+ namespaceIn + "/models/block";

		for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
			if (entry.is_directory()) continue;
			std::string name = namespaceIn + ":" + entry.path().filename().string().substr(0, entry.path().filename().string().length() - 5);//epic one liner
			if (BlockIDNameData.count(name)) {
				continue;
			}

			allOtherBlocks.push_back(name);
		}

		for (string& name : allOtherBlocks) {
			RegisterBlock(name, new MaterialNone(), false, true, false);
		}

	}
	catch (std::filesystem::filesystem_error& e) {
		Logger.LogError("File System", e.what());
	}
}


void BlockList::InitializeBlockModels()  {
	//add every single block  in the assets bc why not
	
	try {
		for (const auto& entry : std::filesystem::directory_iterator("assets")) {
			if (!entry.is_directory()) continue;
			std::string name = entry.path().filename().string();//epic one liner
			if (!strcmp(name.c_str(), "shaders")) continue;
			AddAssets(name);
		}

	}
	catch (std::filesystem::filesystem_error& e) {
		Logger.LogError("File System", e.what());
	}

	FastHashMap<std::string, int> TextureIDs;
	FastHashMap<int, int> TextureRepeatCount;
	FastHashMap<int, bool> TextureTransparency;
	FastHashMap<int, bool> TextureSeeThrough;
 
	//It will first go through the block models and create the models without loading the texture
	for (const auto& [Name, ID] : BlockIDNameData) {
		auto Tokens = Tokenize(Name, ':');

		ModelV2::BlockModelV2* model = getBlockModel("block/" + Tokens.back(), Tokens.front());
		model->isInitialized = true;
		getBlockType(ID)->BlockModelData = model;
	}

	//Bake all of the texture variables next

	for (auto& block : BlockTypeData) {
		if (block->BlockModelData == NULL) continue;
		block->BlockModelData->flattenVariables();
	}

	//Bake all rotations into the UV
	for (auto& block : BlockTypeData) {
		if (block->BlockModelData == NULL) continue;
		block->BlockModelData->bakeTextureRotation();
	}

	//Generate all the textures now
	for (auto& block  : BlockTypeData) {
		if (block->BlockModelData == NULL) continue;

		for (auto& element : block->BlockModelData->Elements) {
			for (int i = 0; i < 6; i++) {
				std::string path = element.Faces[i].ReferenceTexture;
				if (path.length() == 0) continue;

				//Check if it is loaded already
				if (TextureIDs.count(path)) {
					element.Faces[i].TextureID = TextureIDs[path];
					element.Faces[i].TextureCount = TextureRepeatCount[element.Faces[i].TextureID];
					element.Faces[i].hasTransparency = TextureTransparency[element.Faces[i].TextureID];
					element.Faces[i].isSeeThrough = TextureSeeThrough[element.Faces[i].TextureID];
					continue; //Exits
				}

				auto Tokens = Tokenize(path, ':');

				//Load texture
				std::string TexFile = "assets/" + Tokens.back() + "/textures/" + Tokens.front() + ".png";

				int NumLayersBefore = BlockTextureAtlas.GetBlockCount();

				// glm::vec4 uv{};

				bool transparency = false;
				bool isSeeThrough = false;

				std::optional<RawTextureData> d = BlockTextureAtlas.AddTextureToAtlas(TexFile, transparency, isSeeThrough);

				if (!d.has_value()) {
					Logger.LogError("Texture Loading", "Unable to load texture");
					continue;
				}

				TextureIDs[path] = NumLayersBefore + 1;
				TextureRepeatCount[NumLayersBefore + 1] = BlockTextureAtlas.GetBlockCount() - NumLayersBefore;
				TextureTransparency[NumLayersBefore + 1] = transparency;
				TextureSeeThrough[NumLayersBefore + 1] = isSeeThrough;


				element.Faces[i].TextureID = TextureIDs[path];
				element.Faces[i].TextureCount = TextureRepeatCount[element.Faces[i].TextureID];
				element.Faces[i].hasTransparency = transparency;
				element.Faces[i].isSeeThrough = isSeeThrough;
			}
		}
	}


	for (int i = 0; i < BlockTypeData.size(); i++) {
		ModelV2::BlockModelV2 model;

		if (BlockTypeData[i]->BlockModelData != NULL) {
			model = *BlockTypeData[i]->BlockModelData;
		}
		model.TextureVariable.clear();
		BlockModelData.emplace_back(model);
	}
}
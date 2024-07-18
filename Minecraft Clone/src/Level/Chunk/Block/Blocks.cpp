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
	BlockID ID = BlockTypeData.size();
	BlockType* NewBlock = new BlockType(transparency, solid, isFluid);

	Block* block = material->BuildNewBlockType();

	MaterialType Type = material->type;

	block->ID = ID;
	block->Properties = NewBlock;
	block->Texture = new BlockTexture;
	block->BlockName = BlockName;

	BlockTypeData.emplace_back(block);

	BlockIDNameData[BlockName] = ID;

	Logger.LogInfo("Register", "Registered new block (ID): " + std::to_string(ID));


	return ID;
}

void BlockList::InitializeTextureV2() {
	//Model path

	std::unordered_map<std::string, int> TextureIDs;
	std::unordered_map<int, int> TextureRepeatCount;

	//Set null
	BlockTextureArray.AddTextureToArray("assets/textures/block/null.png");
	TextureIDs["assets/textures/block/null.png"] = 1;
	TextureRepeatCount[TextureIDs["assets/textures/block/null.png"]] = 1;

	std::string path = "assets/models/block/";

	for (const auto& block : BlockIDNameData) {

		std::string jsonPath = path + block.first + ".json";
		
		if (!FileManager::CheckFile(jsonPath.c_str())) {
			Logger.LogError("Block" , "JSON file doesn't exist: " + jsonPath);
			continue; //Skips if the json doesn't exist
		}

		json BlockResourceData;

		try {
			ifstream file(jsonPath);
			BlockResourceData = json::parse(file);
		}
		catch (exception& e) {
			Logger.LogDebug("JSON", jsonPath + ":" + std::string(e.what()));
			continue;
		}

		json Texture;
		
		for (auto& item : BlockResourceData.items()) {
			if (item.key() == "textures") {
				Texture = item.value();
				break;
			}
		}

		for (auto& side : Texture.items()) {
			std::string pos = side.key();
			std::string str = side.value().items().begin().value();
			
			std::vector<std::string> tokens = Tokenize(str, '/');

			if (tokens.size() != 2) {
				continue;
			}

			std::string TexFile ="assets/textures/block/" + tokens[1] + ".png";

			int NumLayersBefore = BlockTextureArray.GetLayers();

			if (!TextureIDs.count(TexFile)) {
				BlockTextureArray.AddTextureToArray(TexFile);
				TextureIDs[TexFile] = NumLayersBefore + 1;
				TextureRepeatCount[TextureIDs[TexFile]] = BlockTextureArray.GetLayers() - NumLayersBefore;
			}

			int sides[6]{ 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };

			int TexID = TextureIDs[TexFile];

			if (!strcmp(pos.c_str(), "top")) {
				sides[0] = TOP;
			}
			else if (!strcmp(pos.c_str(), "bottom")) {
				sides[0] = BOTTOM;
			}
			else if (!strcmp(pos.c_str(), "side")) {
				sides[0] = LEFT;
				sides[1] = RIGHT;
				sides[2] = FRONT;
				sides[3] = BACK;
			}
			else if (!strcmp(pos.c_str(), "end")) {
				sides[0] = TOP;
				sides[1] = BOTTOM;
			} 
			else if (!strcmp(pos.c_str(), "all")) {
				sides[0] = LEFT;
				sides[1] = RIGHT;
				sides[2] = FRONT;
				sides[3] = BACK;
				sides[4] = TOP;
				sides[5] = BOTTOM;
			}
			else if (!strcmp(pos.c_str(), "pattern")) {
				sides[0] = LEFT; 
				sides[1] = RIGHT;
				sides[2] = FRONT;
				sides[3] = BACK;
				sides[4] = TOP;
				sides[5] = BOTTOM;
			}
			else if (!strcmp(pos.c_str(), "texture")) {
				sides[0] = LEFT;
				sides[1] = RIGHT;
				sides[2] = FRONT;
				sides[3] = BACK;
				sides[4] = TOP;
				sides[5] = BOTTOM;
			}
			else if (!strcmp(pos.c_str(), "down")) {
				sides[0] = BOTTOM;
			}
			else if (!strcmp(pos.c_str(), "east")) {
				sides[0] = RIGHT;
			}
			else if (!strcmp(pos.c_str(), "north")) {
				sides[0] = FRONT;
			}
			else if (!strcmp(pos.c_str(), "south")) {
				sides[0] = BACK;
			}
			else if (!strcmp(pos.c_str(), "up")) {
				sides[0] = TOP;
			}
			else if (!strcmp(pos.c_str(), "west")) {
				sides[0] = LEFT;
			}
			else if (!strcmp(pos.c_str(), "particle")) {
				BlockTypeData[block.second]->Texture->SetParticle(TexID);
				if (Blocks.getBlockType(block.second)->Properties->isFluid) {
					sides[0] = LEFT;
					sides[1] = RIGHT;
					sides[2] = FRONT;
					sides[3] = BACK;
					sides[4] = TOP;
					sides[5] = BOTTOM; //TEMP FIX 
				}

				
			}
			else {
				//idk
			}

			
			BlockTypeData[block.second]->Texture->SetFacesCustom(TexID, TextureRepeatCount[TexID], sides[0], sides[1], sides[2], sides[3], sides[4], sides[5]);
		}
	}
}



void BlockList::InitializeBlockModels()  {
	//add every single block  in the assets bc why not
	try {
		std::vector<std::string> allOtherBlocks{};
		std::string path = "assets/models/block";
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			std::string name = entry.path().filename().string().substr(0, entry.path().filename().string().length() - 5);//epic one liner
			if (BlockIDNameData.count(name)) {
				continue;
			}
			allOtherBlocks.push_back(name);
		}

		for (string& name : allOtherBlocks) {
			RegisterBlock(name, new MaterialNone(), false, true, false);
		}

	}
	catch(std::filesystem::filesystem_error& e) {
		Logger.LogError("File System", e.what());
	}

	

	ska::flat_hash_map<std::string, int> TextureIDs;
	ska::flat_hash_map<int, int> TextureRepeatCount;
	ska::flat_hash_map<int, bool> TextureTransparency;

	//It will first go through the block models and create the models without loading the texture
	for (const auto& [Name, ID] : BlockIDNameData) {
		ModelV2::BlockModelV2* model = getBlockModel(Name);
		getBlockType(ID)->BlockModelData = model;
	}

	//Bake all of the texture variables next

	for (auto& block : BlockTypeData) {
		if (block->BlockModelData == NULL) continue;
		block->BlockModelData->flattenVariables();
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
					continue; //Exits
				}

				//Load texture
				std::string TexFile = "assets/textures/block/" + path + ".png";

				int NumLayersBefore = BlockTextureArray.GetLayers();

				glm::vec4 uv{};

				std::optional<RawTextureData> d = BlockTextureArray.AddTextureToArray(TexFile);
				BlockTextureAtlas.AddTextureToAtlas(TexFile, uv);

				if (!d.has_value())
					continue;


				TextureIDs[path] = NumLayersBefore + 1;
				TextureRepeatCount[NumLayersBefore + 1] = BlockTextureArray.GetLayers() - NumLayersBefore;
				TextureTransparency[NumLayersBefore + 1] = d.value().format == GL_RGBA;

				element.Faces[i].TextureID = TextureIDs[path];
				element.Faces[i].TextureCount = TextureRepeatCount[element.Faces[i].TextureID];
				element.Faces[i].hasTransparency = d.value().format == GL_RGBA;

			}
		}

	}
}
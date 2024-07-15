#include "Blocks.h"
#include "Type/BlockTypes.h"
#include <nlohmann/json.hpp>
#include "../../../FileManager/Files.h"
#include <fstream>
#include "../../../RenderEngine/BlockModel/ModelLoader.h"

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



void BlockList::InitializeBlockModels() {
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


}
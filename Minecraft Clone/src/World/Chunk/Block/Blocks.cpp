#include "Blocks.h"
#include "Type/BlockTypes.h"
#include <nlohmann/json.hpp>

#include <fstream>

using namespace std;

using json = nlohmann::json;

BlockID BlockList::RegisterBlock(std::string BlockName, Material* material, bool transparency, bool solid, bool isFluid) {
	BlockID blockID = BlockTypeCount;
	BlockID ID = BlockTypeCount;
	BlockType* NewBlock = new BlockType(transparency, solid, isFluid);

	Block* block = material->BuildNewBlockType();

	MaterialType Type = material->type;

	block->ID = ID;
	block->Properties = NewBlock;
	block->Texture = new BlockTexture;
	block->BlockName = BlockName;

	BlockTypeData[ID] = block;

	BlockIDNameData[BlockName] = ID;

	Logger.LogInfo("Register", "Registered new block (ID): " + std::to_string(ID));

	BlockTypeCount++;

	return BlockTypeCount - 1;
}

void BlockList::InitializeTextures() {
	ifstream file("assets/BlockTextures.json");

	json TextureData = json::parse(file);

	std::unordered_map<std::string, int> TextureIDs;

	for (auto& b : TextureData.items()) { //Block

		BlockID id = BlockIDNameData[b.key()];

		for (auto& texture : b.value().items().begin().value().items()) {

			json::iterator it = texture.value().begin();

			string TexFile = it.value();

			if (!TextureIDs.count(TexFile)) {
				BlockTextureArray.AddTextureToArray(TexFile);
				TextureIDs[TexFile] = BlockTextureArray.GetLayers();
			}

			it++;

			int sides[6]{ 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };

			int i = 0;

			for (auto side : it.value().items()) {
				int s = 0xFF;

				string texSide = side.value();

				if (!strcmp(texSide.c_str(), "FRONT")) {
					s = FRONT;
				}
				if (!strcmp(texSide.c_str(), "BACK")) {
					s = BACK;
				}
				if (!strcmp(texSide.c_str(), "LEFT")) {
					s = LEFT;
				}	
				if (!strcmp(texSide.c_str(), "RIGHT")) {
					s = RIGHT;
				}
				if (!strcmp(texSide.c_str(), "TOP")) {
					s = TOP;
				}
				if (!strcmp(texSide.c_str(), "BOTTOM")) {
					s = BOTTOM;
				}

				sides[i] = s;
				i++;	
			}
			
			int TexID = TextureIDs[TexFile];

			BlockTypeData[id]->Texture->SetFacesCustom(TexID, sides[0], sides[1], sides[2], sides[3], sides[4], sides[5]);
		}

	}
}
#include "BlockModels.h"
#include <unordered_set>
#include "../../Utils/LogUtils.h"

void ModelV2::BlockModelV2::flattenVariables() {
	std::unordered_map<std::string, std::string> VariableMatcher{};
	std::unordered_set<std::string> variableNames{};
	

	//This will set all of the texture variables

	for (const auto& [Key, Value] : TextureVariable) {
		//Check if there is a # at the beginning
		variableNames.insert('#' + Key);
		if (Value[0] == '#') {
			variableNames.insert(Value);
		}
		//In this case, i am inserting both key and values because a variable could be referencing another variable
	}

	//Then it will find the absolute value corresponding to those variables


	for (const std::string& variable : variableNames) {
		std::string TextureName = variable;
		std::vector<std::string> variableCorrspondingToSameTexture = {};

		std::unordered_set<std::string> variableVisitMap{};


		int i = 0;
		while (TextureName[0] == '#') {
			if (VariableMatcher.count(TextureName)) { //top down memorization
				TextureName = VariableMatcher[TextureName];
				break;
			}
			if (i > 10000) { //too lazy to fix infinite loop  issues for now
				break;
			}
			i++;
			variableCorrspondingToSameTexture.push_back(TextureName);
			TextureName = TextureVariable[TextureName.substr(1, TextureName.length() - 1)];
		}

		for (const std::string& var : variableCorrspondingToSameTexture) {
			VariableMatcher[var] = TextureName;
		}
		variableCorrspondingToSameTexture.clear();
	}

	//After all of the texture are flatten it will go through the faces and set the textures
	for (Cuboid& element : Elements) {
		for (BlockFace& face : element.Faces) {
			if (face.ReferenceTexture.length() == 0) continue; //if there is no texture face is not initialized (skip)

			face.ReferenceTexture = VariableMatcher[face.ReferenceTexture];
		}
	}
}
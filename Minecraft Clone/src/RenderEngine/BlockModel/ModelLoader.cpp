#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "ModelLoader.h"
#include "../../FileManager/Files.h"
#include "../../Utils/LogUtils.h"

const std::string DEFAULT_NAMESPACE = "minecraft";

using json = nlohmann::json;
using namespace std;


vector<int> getJSONArrayValues(json JsonData) {
	vector<int> arr{};

	for (auto& value : JsonData.items()) {	
		arr.push_back(value.value());
	}

	return arr;
}

vector<float> getJSONArrayValuesFloat(json JsonData) {
	vector<float> arr{};

	for (auto& value : JsonData.items()) {
		arr.push_back(value.value());
	}

	return arr;
}

int ConvertStringFaceToIndex(const string& str) {
	if (!strcmp(str.c_str(), "down")) {
		return DOWN;
	}
	else if (!strcmp(str.c_str(), "up")) {
		return UP;
	}
	else if (!strcmp(str.c_str(), "north")) {
		return NORTH;
	}
	else if (!strcmp(str.c_str(), "south")) {
		return SOUTH;
	}
	else if (!strcmp(str.c_str(), "west")) {
		return WEST;
	}
	else if (!strcmp(str.c_str(), "east")) {
		return EAST;
	}
	else {
		Logger.LogError("Model Loader", "Unknown direction: " + str);
		return NULL;
	}

}

void ProcessSingleCubeFaces(Cuboid& cube, json JsonData) {
	for (auto& face : JsonData.items()) {
		int faceIndex = ConvertStringFaceToIndex(face.key());
		BlockFace bFace;
		for (auto& faceElements : face.value().items()) {

			if (!strcmp(faceElements.key().c_str(), "uv")) {
				vector<int> arr = getJSONArrayValues(faceElements.value());

				//flip
				arr[1] = 16 - arr[1];
				arr[3] = 16 - arr[3];

				std::swap(arr[1], arr[3]);

				for (int i = 0; i < 4; i++) {
					bFace.UV[i] = arr[i];
				}
			}
			else if (!strcmp(faceElements.key().c_str(), "texture")) {
				auto Tokens = Tokenize(faceElements.value(),  ':');

				string TexName = Tokens.back();
				string TexNamespace = DEFAULT_NAMESPACE;

				if (Tokens.size() == 2) {
					TexNamespace = Tokens.front();
				}

				bool isVariable = TexName[0] == '#';

				if (isVariable) {
					bFace.ReferenceTexture = TexName;
				}
				else {
					bFace.ReferenceTexture = TexName + ":" + TexNamespace;
				}
			}
			else if (!strcmp(faceElements.key().c_str(), "cullface")) {
				bFace.CullFace = ConvertStringFaceToIndex(faceElements.value()) - 1;
			}
			else if (!strcmp(faceElements.key().c_str(), "tintindex")) {
				bFace.TintIndex = faceElements.value();
			}
			else if (!strcmp(faceElements.key().c_str(), "rotation")) {
				bFace.rotation = faceElements.value();
			}
			else {
				Logger.LogError("Model Loader", "Unknown face attribute: " + faceElements.key());
			}


		}
		cube.EditFace(faceIndex, bFace);
	}
}

CuboidRotationInfo getRotationalData(json JsonData) {
	CuboidRotationInfo rotationInfo;
	for (auto& attribute : JsonData.items()) {
		if (!strcmp(attribute.key().c_str(), "origin")) {
			vector<float> arr = getJSONArrayValuesFloat(attribute.value());

			for (int i = 0; i < 3; i++) {
				rotationInfo.origin[i] = arr[i];
			}
		}
		else if (!strcmp(attribute.key().c_str(), "axis")) {
			char axis = static_cast<string>(attribute.value())[0];

			if (axis == 'x') {
				rotationInfo.axis = 0;
			}
			else if (axis == 'y') {
				rotationInfo.axis = 1;
			}
			else if (axis == 'z') {
				rotationInfo.axis = 2;
			}
			else {
				Logger.LogError("Model Loader", "Unknown rotational axis: " + attribute.value());
			}
			
		}
		else if (!strcmp(attribute.key().c_str(), "axis")) {
			int angle = attribute.value();
			rotationInfo.angle = angle;
		}
		else if (!strcmp(attribute.key().c_str(), "rescale")) {
			rotationInfo.rescale = true;
		}
		rotationInfo.Initialized = true;
	}

	return rotationInfo;
}

void UpdateModelElements(ModelV2::BlockModelV2* model, json JsonData) {
	for (auto& item : JsonData.items()) {

		Cuboid cuboid;

		for (auto& subElements : item.value().items()) {

			if (!strcmp(subElements.key().c_str(), "from")) {
				vector<int> arr = getJSONArrayValues(subElements.value());
				for (int i = 0; i < 3; i++) {
					cuboid.From[i] = arr[i];
				}
				arr.clear();
			}
			else if (!strcmp(subElements.key().c_str(), "to")) {
				vector<int> arr = getJSONArrayValues(subElements.value());
				for (int i = 0; i < 3; i++) {
					cuboid.To[i] = arr[i];
				}
				arr.clear();
			}
			else if (!strcmp(subElements.key().c_str(), "faces")) {
				ProcessSingleCubeFaces(cuboid, subElements.value());
			}
			else if (!strcmp(subElements.key().c_str(), "rotation")) { 
				CuboidRotationInfo rotation = getRotationalData(subElements.value());
				cuboid.rotation = rotation;
			}
			else if (!strcmp(subElements.key().c_str(), "__comment")) {
				cuboid.comments = subElements.value();
			}
			else if (!strcmp(subElements.key().c_str(), "shade")) {
				cuboid.shade = static_cast<bool>(subElements.value());
			}
			else {
				Logger.LogError("Model Loader", "Unknown element attribute: " + subElements.key());
			}
		}

		model->AddElement(cuboid);
	}
}

void ProcessCuboidTexture(ModelV2::BlockModelV2* model, json JsonData) {
	for (auto& TextureElement : JsonData.items()) {
		auto Tokens = Tokenize(TextureElement.value(), ':');

		string TextureName = Tokens.back();
		string TextureNamespace = Tokens.front();
		

		if (Tokens.size() == 1) {
			TextureNamespace = DEFAULT_NAMESPACE;
		}

		string TextureVariableName = TextureElement.key();

		bool isVariable = TextureName[0] == '#';

		if (isVariable) {
			model->TextureVariable[TextureVariableName] = TextureName;
		}
		else {
			model->TextureVariable[TextureVariableName] = TextureName + ":" + TextureNamespace;
		}

		
	}
}

void ProcessModelDisplay(ModelV2::BlockModelV2* model, json JsonData) {
	for (auto& displayPlaces : JsonData.items()) {

		string position = displayPlaces.key();
		BlockDisplay display;

		for (auto& transitions : displayPlaces.value().items()) {
			vector<float> arr = getJSONArrayValuesFloat(transitions.value());

			if (!strcmp(transitions.key().c_str(), "rotation")) {
				for (int i = 0; i < 3; i++) {
					display.rotation[i] = arr[i];
				}
			}
			else if (!strcmp(transitions.key().c_str(), "translation")) {
				for (int i = 0; i < 3; i++) {
					display.translation[i] = arr[i];
				}
			}
			else if (!strcmp(transitions.key().c_str(), "scale")) {
				for (int i = 0; i < 3; i++) {
					display.scale[i] = arr[i];
				}
			}
			else {
				Logger.LogError("Model Loader", "Unknown display attribute: " + transitions.key());
			}
		}

		if (!strcmp(position.c_str(), "thirdperson_righthand")) {
			display.position = DisplayPosition::thirdperson_righthand;
		}
		else if (!strcmp(position.c_str(), "thirdperson_lefthand")) {
			display.position = DisplayPosition::thirdperson_lefthand;
		}
		else if (!strcmp(position.c_str(), "firstperson_righthand")) {
			display.position = DisplayPosition::firstperson_righthand;
		}
		else if (!strcmp(position.c_str(), "firstperson_lefthand")) {
			display.position = DisplayPosition::firstperson_lefthand;
		}
		else if (!strcmp(position.c_str(), "gui")) {
			display.position = DisplayPosition::gui;
		}
		else if (!strcmp(position.c_str(), "head")) {
			display.position = DisplayPosition::head;
		}
		else if (!strcmp(position.c_str(), "ground")) {
			display.position = DisplayPosition::ground;
		}
		else if (!strcmp(position.c_str(), "fixed")) {
			display.position = DisplayPosition::fixed;
		}
		else {
			Logger.LogError("Model Loader", "Unknown display position: " + position);
			return;
		}

		display.Initialized = true;

		model->AddDisplay(display, display.position);
	}
}

ModelV2::BlockModelV2* recursiveGetBlockModel(string jsonName, string namespaceIn) {
	ModelV2::BlockModelV2* model = nullptr;

	string jsonPath = "assets/" + namespaceIn + "/models/" + jsonName + ".json";
	
	json JSONData;

	try {
		ifstream file(jsonPath);
		if (!file.good()) { //checks if  it exist
			return nullptr;
		}
		JSONData = json::parse(file);
	}
	catch (exception& e) {
		Logger.LogError("Model Loader", "Path doesn't exist: " + jsonPath);
		return nullptr;
	}

	//Search for the parent
	for (auto& item : JSONData.items()) {
		if (item.key() == "parent") {
			string ParentData = item.value();

			//Parse for the parent json

			vector<string> Tokens = Tokenize(ParentData, ':'); //This assumes that the parent is always in the same folder
			string ParentJSON = Tokens.back();
			string ParentNamespace = Tokens.front();
			
			if (Tokens.size() == 1) { //default namespace is minecraft
				ParentNamespace = DEFAULT_NAMESPACE;
			}

			model = recursiveGetBlockModel(ParentJSON, ParentNamespace);

			break;
		}
	}

	if (model == nullptr)
		model = new ModelV2::BlockModelV2();

	//Searches for the other stuff like textures and elements 

	for (auto& item : JSONData.items()) {
		if (!strcmp(item.key().c_str(), "elements")) {
			model->Elements.clear(); //Child elements override parent elements
			UpdateModelElements(model, item.value());
		}
		else if (!strcmp(item.key().c_str(), "textures")) {
			ProcessCuboidTexture(model, item.value());
		}
		else if (!strcmp(item.key().c_str(), "display")) {
			ProcessModelDisplay(model, item.value());
		}
		else if (!strcmp(item.key().c_str(), "ambientocclusion")) {
			model->AmbientOcclusion = static_cast<bool>(item.value());
		}
	}

	return model;
}

ModelV2::BlockModelV2* getBlockModel(string blockNameIn, string namespaceIn) {
	//This will recursively go into parents files and build on it
	ModelV2::BlockModelV2* model = recursiveGetBlockModel(blockNameIn, namespaceIn);
	return model;

}
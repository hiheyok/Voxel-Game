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
		g_logger.LogError("Model Loader", "Unknown direction: " + str);
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
					bFace.uv_[i] = arr[i];
				}
			}
			else if (!strcmp(faceElements.key().c_str(), "texture")) {
				auto tokens = Tokenize(faceElements.value(),  ':');

				string texName = tokens.back();
				string texNamespace = DEFAULT_NAMESPACE;

				if (tokens.size() == 2) {
					texNamespace = tokens.front();
				}

				bool isVariable = texName[0] == '#';

				if (isVariable) {
					bFace.reference_texture_ = texName;
				}
				else {
					bFace.reference_texture_ = texName + ":" + texNamespace;
				}
			}
			else if (!strcmp(faceElements.key().c_str(), "cullface")) {
				bFace.cull_face_ = ConvertStringFaceToIndex(faceElements.value()) - 1;
			}
			else if (!strcmp(faceElements.key().c_str(), "tintindex")) {
				bFace.tint_index_ = faceElements.value();
			}
			else if (!strcmp(faceElements.key().c_str(), "rotation")) {
				bFace.rotation_ = faceElements.value();
			}
			else {
				g_logger.LogError("Model Loader", "Unknown face attribute: " + faceElements.key());
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
				rotationInfo.origin_[i] = arr[i];
			}
		}
		else if (!strcmp(attribute.key().c_str(), "axis")) {
			char axis_ = static_cast<string>(attribute.value())[0];

			if (axis_ == 'x') {
				rotationInfo.axis_ = 0;
			}
			else if (axis_ == 'y') {
				rotationInfo.axis_ = 1;
			}
			else if (axis_ == 'z') {
				rotationInfo.axis_ = 2;
			}
			else {
				g_logger.LogError("Model Loader", "Unknown rotational axis: " + attribute.value());
			}
			
		}
		else if (!strcmp(attribute.key().c_str(), "axis")) {
			int angle_ = attribute.value();
			rotationInfo.angle_ = angle_;
		}
		else if (!strcmp(attribute.key().c_str(), "rescale")) {
			rotationInfo.rescale_ = true;
		}
		rotationInfo.initialized_ = true;
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
					cuboid.from_[i] = arr[i];
				}
				arr.clear();
			}
			else if (!strcmp(subElements.key().c_str(), "to")) {
				vector<int> arr = getJSONArrayValues(subElements.value());
				for (int i = 0; i < 3; i++) {
					cuboid.to_[i] = arr[i];
				}
				arr.clear();
			}
			else if (!strcmp(subElements.key().c_str(), "faces")) {
				ProcessSingleCubeFaces(cuboid, subElements.value());
			}
			else if (!strcmp(subElements.key().c_str(), "rotation")) { 
				CuboidRotationInfo rotation = getRotationalData(subElements.value());
				cuboid.rotation_ = rotation;
			}
			else if (!strcmp(subElements.key().c_str(), "__comment")) {
				cuboid.comments_ = subElements.value();
			}
			else if (!strcmp(subElements.key().c_str(), "shade")) {
				cuboid.shade_ = static_cast<bool>(subElements.value());
			}
			else {
				g_logger.LogError("Model Loader", "Unknown element attribute: " + subElements.key());
			}
		}

		model->AddElement(cuboid);
	}
}

void ProcessCuboidTexture(ModelV2::BlockModelV2* model, json JsonData) {
	for (auto& TextureElement : JsonData.items()) {
		auto tokens = Tokenize(TextureElement.value(), ':');

		string textureName = tokens.back();
		string textureNamespace = tokens.front();
		

		if (tokens.size() == 1) {
			textureNamespace = DEFAULT_NAMESPACE;
		}

		string textureVariableName = TextureElement.key();

		bool isVariable = textureName[0] == '#';

		if (isVariable) {
			model->texture_variable_[textureVariableName] = textureName;
		}
		else {
			model->texture_variable_[textureVariableName] = textureName + ":" + textureNamespace;
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
					display.rotation_[i] = arr[i];
				}
			}
			else if (!strcmp(transitions.key().c_str(), "translation")) {
				for (int i = 0; i < 3; i++) {
					display.translation_[i] = arr[i];
				}
			}
			else if (!strcmp(transitions.key().c_str(), "scale")) {
				for (int i = 0; i < 3; i++) {
					display.scale_[i] = arr[i];
				}
			}
			else {
				g_logger.LogError("Model Loader", "Unknown display attribute: " + transitions.key());
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
			g_logger.LogError("Model Loader", "Unknown display position: " + position);
			return;
		}

		display.initialized_ = true;

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
	catch (const exception& e) {
		g_logger.LogError("Model Loader", e.what());
		return nullptr;
	}

	//Search for the parent
	for (auto& item : JSONData.items()) {
		if (item.key() == "parent") {
			string parentData = item.value();

			//Parse for the parent json

			vector<string> tokens = Tokenize(parentData, ':'); //This assumes that the parent is always in the same folder
			string parentJSON = tokens.back();
			string parentNamespace = tokens.front();
			
			if (tokens.size() == 1) { //default namespace is minecraft
				parentNamespace = DEFAULT_NAMESPACE;
			}

			model = recursiveGetBlockModel(parentJSON, parentNamespace);

			break;
		}
	}

	if (model == nullptr)
		model = new ModelV2::BlockModelV2();

	//Searches for the other stuff like textures and elements 

	for (auto& item : JSONData.items()) {
		if (!strcmp(item.key().c_str(), "elements")) {
			model->elements_.clear(); //Child elements override parent elements
			UpdateModelElements(model, item.value());
		}
		else if (!strcmp(item.key().c_str(), "textures")) {
			ProcessCuboidTexture(model, item.value());
		}
		else if (!strcmp(item.key().c_str(), "display")) {
			ProcessModelDisplay(model, item.value());
		}
		else if (!strcmp(item.key().c_str(), "ambientocclusion")) {
			model->ambient_occlusion_ = static_cast<bool>(item.value());
		}
	}

	return model;
}

ModelV2::BlockModelV2* getBlockModel(string blockNameIn, string namespaceIn) {
	//This will recursively go into parents files and build on it
	ModelV2::BlockModelV2* model = recursiveGetBlockModel(blockNameIn, namespaceIn);
	std::reverse(model->elements_.begin(), model->elements_.end());
	return model;

}
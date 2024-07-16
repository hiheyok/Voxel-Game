#include <nlohmann/json.hpp>
#include <fstream>

#include "ModelLoader.h"
#include "../../FileManager/Files.h"
#include "../../Utils/LogUtils.h"

using json = nlohmann::json;
using namespace std;


const string path = "assets/models/block/";


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
				bFace.ReferenceTexture = faceElements.value();
			}
			else if (!strcmp(faceElements.key().c_str(), "cullface")) {
				bFace.CullFace = ConvertStringFaceToIndex(faceElements.value()) - 1;
			}
			else if (!strcmp(faceElements.key().c_str(), "tintindex")) {
				bFace.TintIndex = faceElements.value();
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
			else {
				Logger.LogError("Model Loader", "Unknown element attribute: " + subElements.key());
			}
		}

		model->AddElement(cuboid);
	}
}

void ProcessCuboidTexture(ModelV2::BlockModelV2* model, json JsonData) {
	for (auto& TextureElement : JsonData.items()) {
		string TextureName = Tokenize(TextureElement.value(), '/').back();
		string TextureVariableName = TextureElement.key();

		model->TextureVariable[TextureVariableName] = TextureName;
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
		}

		display.Initialized = true;

		model->AddDisplay(display, display.position);
	}
}

ModelV2::BlockModelV2* recursiveGetBlockModel(string jsonName) {
	ModelV2::BlockModelV2* model = nullptr;

	string jsonPath = path + jsonName + ".json";
	//Check if the file exist, if it doesn't thrown an error
	if (!FileManager::CheckFile(jsonPath.c_str())) {
		return nullptr;
	}

	json JSONData;

	try {
		ifstream file(jsonPath);
		JSONData = json::parse(file);
	}
	catch (exception& e) {
		return nullptr;
	}

	//Search for the parent
	for (auto& item : JSONData.items()) {
		if (item.key() == "parent") {
			string ParentData = item.value();

			//Parse for the parent json

			vector<string> Tokens = Tokenize(ParentData, '/'); //This assumes that the parent is always in the same folder
			string ParentJSON = Tokens.back();
			model = recursiveGetBlockModel(ParentJSON);
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
	}

	return model;
}

ModelV2::BlockModelV2* getBlockModel(string blockNameIn) {
	//This will recursively go into parents files and build on it
	ModelV2::BlockModelV2* model = recursiveGetBlockModel(blockNameIn);
	//std::reverse(model->Elements.begin(), model->Elements.end());
	return model;

}
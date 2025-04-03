#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "RenderEngine/BlockModel/ModelLoader.h"
#include "FileManager/Files.h"
#include "Utils/LogUtils.h"

const std::string DEFAULT_NAMESPACE = "minecraft";

using json = nlohmann::json;

static std::vector<int> getJSONArrayValues(json JsonData) {
    std::vector<int> arr{};

    for (auto& value : JsonData.items()) {    
        arr.push_back(value.value());
    }

    return arr;
}

static std::vector<float> getJSONArrayValuesFloat(json JsonData) {
    std::vector<float> arr{};

    for (auto& value : JsonData.items()) {
        arr.push_back(value.value());
    }

    return arr;
}

static int ConvertStringFaceToIndex(const std::string& str) {
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
        g_logger.LogError("ConvertStringFaceToIndex", "Unknown direction: " + str);
        return 0;
    }

}

static void ProcessSingleCubeFaces(Cuboid& cube, json JsonData) {
    for (auto& face : JsonData.items()) {
        int faceIndex = ConvertStringFaceToIndex(face.key());
        BlockFace bFace;
        for (auto& faceElements : face.value().items()) {

            if (!strcmp(faceElements.key().c_str(), "uv")) {
                std::vector<int> arr = getJSONArrayValues(faceElements.value());

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

                std::string texName = tokens.back();
                std::string texNamespace = DEFAULT_NAMESPACE;

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
                g_logger.LogError("ProcessSingleCubeFaces", "Unknown face attribute: " + faceElements.key());
            }


        }
        cube.EditFace(faceIndex, bFace);
    }
}

static CuboidRotationInfo getRotationalData(json JsonData) {
    CuboidRotationInfo rotationInfo;
    for (auto& attribute : JsonData.items()) {
        if (!strcmp(attribute.key().c_str(), "origin")) {
            std::vector<float> arr = getJSONArrayValuesFloat(attribute.value());

            for (int i = 0; i < 3; i++) {
                rotationInfo.origin_[i] = arr[i];
            }
        }
        else if (!strcmp(attribute.key().c_str(), "axis")) {
            char axis_ = static_cast<std::string>(attribute.value())[0];

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
                g_logger.LogError("getRotationalData", "Unknown rotational axis: " + attribute.value());
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

static void UpdateModelElements(std::unique_ptr<Model::BlockModel>& model, json JsonData) {
    for (auto& item : JsonData.items()) {

        Cuboid cuboid;

        for (auto& subElements : item.value().items()) {

            if (!strcmp(subElements.key().c_str(), "from")) {
                std::vector<int> arr = getJSONArrayValues(subElements.value());
                for (int i = 0; i < 3; i++) {
                    cuboid.from_[i] = arr[i];
                }
                arr.clear();
            }
            else if (!strcmp(subElements.key().c_str(), "to")) {
                std::vector<int> arr = getJSONArrayValues(subElements.value());
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
                g_logger.LogError("UpdateModelElements", "Unknown element attribute: " + subElements.key());
            }
        }

        model->AddElement(cuboid);
    }
}

static void ProcessCuboidTexture(std::unique_ptr<Model::BlockModel>& model, json JsonData) {
    for (auto& TextureElement : JsonData.items()) {
        auto tokens = Tokenize(TextureElement.value(), ':');

        std::string textureName = tokens.back();
        std::string textureNamespace = tokens.front();
        

        if (tokens.size() == 1) {
            textureNamespace = DEFAULT_NAMESPACE;
        }

        std::string textureVariableName = TextureElement.key();

        bool isVariable = textureName[0] == '#';

        if (isVariable) {
            model->texture_variable_[textureVariableName] = textureName;
        }
        else {
            model->texture_variable_[textureVariableName] = textureName + ":" + textureNamespace;
        }

        
    }
}

static void ProcessModelDisplay(std::unique_ptr<Model::BlockModel>& model, json JsonData) {
    for (auto& displayPlaces : JsonData.items()) {

        std::string position = displayPlaces.key();
        BlockDisplay display;

        for (auto& transitions : displayPlaces.value().items()) {
            std::vector<float> arr = getJSONArrayValuesFloat(transitions.value());

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
                g_logger.LogError("ProcessModelDisplay", "Unknown display attribute: " + transitions.key());
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
            g_logger.LogError("ProcessModelDisplay", "Unknown display position: " + position);
            return;
        }

        display.initialized_ = true;

        model->AddDisplay(display, display.position);
    }
}

//TODO: Work on caching
static std::unique_ptr<Model::BlockModel> recursiveGetBlockModel(std::string jsonName, std::string namespaceIn) {
    std::unique_ptr<Model::BlockModel> model = nullptr;

    std::string jsonPath = "assets/" + namespaceIn + "/models/" + jsonName + ".json";
    
    json JSONData;

    try {
        std::ifstream file(jsonPath);
        if (!file.good()) { //checks if  it exist
            return nullptr;
        }
        JSONData = json::parse(file);
    }
    catch (const std::exception& e) {
        g_logger.LogError("recursiveGetBlockModel", e.what());
        return nullptr;
    }

    //Search for the parent
    for (auto& item : JSONData.items()) {
        if (item.key() == "parent") {
            std::string parentData = item.value();

            //Parse for the parent json

            std::vector<std::string> tokens = Tokenize(parentData, ':'); //This assumes that the parent is always in the same folder
            std::string parentJSON = tokens.back();
            std::string parentNamespace = tokens.front();
            
            if (tokens.size() == 1) { //default namespace is minecraft
                parentNamespace = DEFAULT_NAMESPACE;
            }

            model = recursiveGetBlockModel(parentJSON, parentNamespace);

            break;
        }
    }

    if (model == nullptr)
        model = std::make_unique<Model::BlockModel>();

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

std::unique_ptr<Model::BlockModel> getBlockModel(std::string blockNameIn, std::string namespaceIn) {
    //This will recursively go into parents files and build on it
    std::unique_ptr<Model::BlockModel> model = recursiveGetBlockModel(blockNameIn, namespaceIn);
    std::reverse(model->elements_.begin(), model->elements_.end());
    return model;

}
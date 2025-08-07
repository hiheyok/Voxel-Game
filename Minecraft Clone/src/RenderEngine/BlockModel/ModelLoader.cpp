// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/BlockModel/ModelLoader.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "FileManager/Files.h"
#include "Utils/LogUtils.h"

ModelLoader::ModelLoader(GameContext& context) : context_{context} {}
ModelLoader::~ModelLoader() = default;

std::unique_ptr<BlockModel> ModelLoader::GetModel(
    const ResourceLocation& location) {
  // Check if the model is already in the cache
  const auto& it = cache_.find(location);

  if (it != cache_.end()) {
    return std::make_unique<BlockModel>(*it->second);
  }

  std::unique_ptr<BlockModel> model = GetModelRecursive(location);
  if (model == nullptr) return nullptr;
  std::reverse(model->elements_.begin(), model->elements_.end());
  CacheModel(location, model);
  return model;
}

std::unique_ptr<BlockModel> ModelLoader::GetModelRecursive(
    const ResourceLocation& location) {
  std::unique_ptr<BlockModel> model = nullptr;

  std::string jsonPath = location.GetPath();
  json JSONData;

  try {
    std::ifstream file(jsonPath);
    if (!file.good()) {  // checks if  it exist
      return nullptr;
    }
    JSONData = json::parse(file);
  } catch (const std::exception& e) {
    LOG_WARN(e.what());
    return nullptr;
  }

  // Search for the parent
  for (auto& item : JSONData.items()) {
    if (item.key() == "parent") {
      std::string parentData = item.value();

      // Parse for the parent json

      std::vector<std::string> tokens =
          Tokenize(parentData, ':');  // This assumes that the parent is
                                      // always in the same folder
      std::string parentJSON = tokens.back();
      std::string parentNamespace = tokens.front();

      if (tokens.size() == 1) {
        parentNamespace = "";
      }

      // First check if the model is in the cache
      ResourceLocation parentLocation =
          ResourceLocation("models/" + parentJSON + ".json", parentNamespace);
      const auto& it = cache_.find(parentLocation);
      if (it != cache_.end()) {
        model = std::make_unique<BlockModel>(*it->second);
      } else {
        model = GetModelRecursive(parentLocation);
        if (model != nullptr) {
          CacheModel(parentLocation, model);
        }
      }

      break;
    }
  }

  if (model == nullptr) {
    model = std::make_unique<BlockModel>();
  }

  // Searches for the other stuff like textures and elements

  for (auto& item : JSONData.items()) {
    if (item.key() == "elements") {
      model->elements_.clear();  // Child elements override parent
                                 // elements
      UpdateModelElements(model, item.value());
    } else if (item.key() == "textures") {
      ProcessCuboidTexture(model, item.value());
    } else if (item.key() == "display") {
      ProcessModelDisplay(model, item.value());
    } else if (item.key() == "ambientocclusion") {
      model->ambient_occlusion_ = static_cast<bool>(item.value());
    }
  }

  return model;
}

void ModelLoader::CacheModel(const ResourceLocation& location,
                             const std::unique_ptr<BlockModel>& model) {
  // Check if the model is already cached
  const auto& it = cache_.find(location);
  if (it != cache_.find(location)) {
    LOG_DEBUG("Attempted to cache model that is already cached");
  }
  std::unique_ptr<BlockModel> copiedModel =
      std::make_unique<BlockModel>(*model);
  cache_[location] = std::move(copiedModel);
}

void ModelLoader::ProcessModelDisplay(std::unique_ptr<BlockModel>& model,
                                      json JsonData) {
  for (auto& displayPlaces : JsonData.items()) {
    std::string position = displayPlaces.key();
    BlockDisplay display;

    for (auto& transitions : displayPlaces.value().items()) {
      std::vector<float> arr = GetJSONArrayValuesFloat(transitions.value());

      if (transitions.key() == "rotation") {
        for (int i = 0; i < 3; i++) {
          display.rotation_[i] = arr[i];
        }
      } else if (transitions.key() == "translation") {
        for (int i = 0; i < 3; i++) {
          display.translation_[i] = arr[i];
        }
      } else if (transitions.key() == "scale") {
        for (int i = 0; i < 3; i++) {
          display.scale_[i] = arr[i];
        }
      } else {
        LOG_WARN("Unknown display attribute: " + transitions.key());
      }
    }

    if (position == "thirdperson_righthand") {
      display.position_ = DisplayPosition::thirdperson_righthand;
    } else if (position == "thirdperson_lefthand") {
      display.position_ = DisplayPosition::thirdperson_lefthand;
    } else if (position == "firstperson_righthand") {
      display.position_ = DisplayPosition::firstperson_righthand;
    } else if (position == "firstperson_lefthand") {
      display.position_ = DisplayPosition::firstperson_lefthand;
    } else if (position == "gui") {
      display.position_ = DisplayPosition::gui;
    } else if (position == "head") {
      display.position_ = DisplayPosition::head;
    } else if (position == "ground") {
      display.position_ = DisplayPosition::ground;
    } else if (position == "fixed") {
      display.position_ = DisplayPosition::fixed;
    } else {
      LOG_WARN("Unknown display position: " + position);
      return;
    }

    display.initialized_ = true;

    model->AddDisplay(display, display.position_);
  }
}

void ModelLoader::UpdateModelElements(std::unique_ptr<BlockModel>& model,
                                      json JsonData) {
  for (auto& item : JsonData.items()) {
    Cuboid cuboid;

    for (auto& subElements : item.value().items()) {
      if (subElements.key() == "from") {
        std::vector<float> arr = GetJSONArrayValuesFloat(subElements.value());
        for (int i = 0; i < 3; i++) {
          cuboid.from_[i] = arr[i] / 16;
        }
        arr.clear();
      } else if (subElements.key() == "to") {
        std::vector<float> arr = GetJSONArrayValuesFloat(subElements.value());
        for (int i = 0; i < 3; i++) {
          cuboid.to_[i] = arr[i] / 16;
        }
        arr.clear();
      } else if (subElements.key() == "faces") {
        ProcessSingleCubeFaces(cuboid, subElements.value());
      } else if (subElements.key() == "rotation") {
        CuboidRotationInfo rotation = GetRotationalData(subElements.value());
        cuboid.rotation_ = rotation;
      } else if (subElements.key() == "__comment") {
        cuboid.comments_ = subElements.value();
      } else if (subElements.key() == "shade") {
        cuboid.shade_ = static_cast<bool>(subElements.value());
      } else {
        LOG_WARN("Unknown element attribute: " + subElements.key());
      }
    }
    model->AddElement(cuboid);
  }
}

void ModelLoader::ProcessCuboidTexture(std::unique_ptr<BlockModel>& model,
                                       json JsonData) {
  for (auto& TextureElement : JsonData.items()) {
    auto tokens = Tokenize(TextureElement.value(), ':');

    std::string textureName = tokens.back();
    std::string textureNamespace = tokens.front();

    if (tokens.size() == 1) {
      textureNamespace = kDefaultNamespace;
    }

    std::string textureVariableName = TextureElement.key();

    bool isVariable = textureName[0] == '#';

    if (isVariable) {
      model->texture_variable_[textureVariableName] = textureName;
    } else {
      model->texture_variable_[textureVariableName] =
          textureName + ":" + textureNamespace;
    }
  }
}

CuboidRotationInfo ModelLoader::GetRotationalData(json JsonData) {
  CuboidRotationInfo rotationInfo;
  for (auto& attribute : JsonData.items()) {
    if (attribute.key() == "origin") {
      std::vector<float> arr = GetJSONArrayValuesFloat(attribute.value());

      for (int i = 0; i < 3; i++) {
        rotationInfo.origin_[i] = arr[i] / 16;
      }
    } else if (attribute.key() == "axis") {
      char axis_ = static_cast<std::string>(attribute.value())[0];

      if (axis_ == 'x') {
        rotationInfo.axis_ = 0;
      } else if (axis_ == 'y') {
        rotationInfo.axis_ = 1;
      } else if (axis_ == 'z') {
        rotationInfo.axis_ = 2;
      } else {
        LOG_WARN("Unknown rotational axis: " +
                 static_cast<std::string>(attribute.value()));
      }
    } else if (attribute.key() == "axis") {
      int angle_ = attribute.value();
      rotationInfo.angle_ = angle_;
    } else if (attribute.key() == "rescale") {
      rotationInfo.rescale_ = true;
    }
    rotationInfo.initialized_ = true;
  }

  return rotationInfo;
}

void ModelLoader::ProcessSingleCubeFaces(Cuboid& cube, json JsonData) {
  for (auto& face : JsonData.items()) {
    int faceIndex = ConvertStringFaceToIndex(face.key());
    BlockFace bFace;
    for (auto& faceElements : face.value().items()) {
      if (faceElements.key() == "uv") {
        std::vector<int> arr = GetJSONArrayValues(faceElements.value());

        // flip
        arr[1] = 16 - arr[1];
        arr[3] = 16 - arr[3];

        std::swap(arr[1], arr[3]);

        for (int i = 0; i < 4; i++) {
          bFace.uv_[i] = arr[i];
        }
      } else if (faceElements.key() == "texture") {
        auto tokens = Tokenize(faceElements.value(), ':');

        std::string texName = tokens.back();
        std::string texNamespace = kDefaultNamespace;

        if (tokens.size() == 2) {
          texNamespace = tokens.front();
        }

        bool isVariable = texName[0] == '#';

        if (isVariable) {
          bFace.reference_texture_ = texName;
        } else {
          bFace.reference_texture_ = texName + ":" + texNamespace;
        }
      } else if (faceElements.key() == "cullface") {
        bFace.cull_face_ = ConvertStringFaceToIndex(faceElements.value());
      } else if (faceElements.key() == "tintindex") {
        bFace.tint_index_ = faceElements.value();
      } else if (faceElements.key() == "rotation") {
        bFace.rotation_ = faceElements.value();
      } else {
        LOG_WARN("Unknown face attribute: " + faceElements.key());
      }
    }
    cube.EditFace(faceIndex, bFace);
  }
}

int ModelLoader::ConvertStringFaceToIndex(const std::string& str) {
  if (str == "down") {
    return DOWN;
  } else if (str == "up") {
    return UP;
  } else if (str == "north") {
    return NORTH;
  } else if (str == "south") {
    return SOUTH;
  } else if (str == "west") {
    return WEST;
  } else if (str == "east") {
    return EAST;
  } else {
    LOG_WARN("Unknown direction: " + str);
    return 0;
  }
}

std::vector<float> ModelLoader::GetJSONArrayValuesFloat(json JsonData) {
  std::vector<float> arr{};

  for (auto& value : JsonData.items()) {
    arr.push_back(value.value());
  }

  return arr;
}

std::vector<int> ModelLoader::GetJSONArrayValues(json JsonData) {
  std::vector<int> arr{};

  for (auto& value : JsonData.items()) {
    arr.push_back(value.value());
  }

  return arr;
}

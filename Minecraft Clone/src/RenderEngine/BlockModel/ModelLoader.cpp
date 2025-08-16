// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/BlockModel/ModelLoader.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/IO/FileUtils.h"
#include "FileManager/Files.h"
#include "Utils/LogUtils.h"

ModelLoader::ModelLoader(GameContext& context) : context_{context} {}
ModelLoader::~ModelLoader() = default;

std::unique_ptr<BlockModel> ModelLoader::GetModel(
    const ResourceLocation& location) {
  std::unique_ptr<BlockModel> model = GetModelRecursive(location);
  return model;
}

std::unique_ptr<BlockModel> ModelLoader::GetModelRecursive(
    const ResourceLocation& location) {
  // Checks if it exist in the cache first
  const auto& it = cache_.find(location);
  if (it != cache_.end()) {
    return std::make_unique<BlockModel>(*it->second);
  }

  std::unique_ptr<BlockModel> model = nullptr;
  std::string path = location.GetPath();
  std::vector<char> manifest_data = FileUtils::ReadFileToBuffer(context_, path);
  json manifest;

  try {
    if (manifest_data.empty()) {
      return nullptr;
    }

    manifest = json::parse(manifest_data);
  } catch (const json::exception& e) {
    LOG_WARN("{}", e.what());
    return nullptr;
  }

  if (manifest.contains("parent")) {
    std::string parent = manifest["parent"].get<std::string>();

    // Parse for the parent json
    // This assumes that the parent is
    // always in the same folder
    size_t idx = parent.find(':');

    std::string parent_location;
    std::string parent_namespace;

    if (idx != std::string::npos) {
      parent_location = parent.substr(idx + 1);
      parent_namespace = parent.substr(0, idx);
    } else {
      parent_location = parent;
      parent_namespace = kDefaultNamespace;
    }

    // First check if the model is in the cache
    ResourceLocation parent_path{"models/" + parent_location + ".json",
                                 parent_namespace};

    model = GetModelRecursive(parent_path);
  }

  if (!model) {
    model = std::make_unique<BlockModel>();
  }

  if (manifest.contains("elements")) {
    // Child elements override parent
    // elements
    model->elements_.clear();
    UpdateModelElements(model, manifest["elements"]);
  }

  if (manifest.contains("textures")) {
    ProcessCuboidTexture(model, manifest["textures"]);
  }

  if (manifest.contains("display")) {
    ProcessModelDisplay(model, manifest["display"]);
  }

  if (manifest.contains("ambientocclusion")) {
    model->ambient_occlusion_ = manifest["ambientocclusion"].get<bool>();
  }

  std::reverse(model->elements_.begin(), model->elements_.end());
  CacheModel(location, model);
  return model;
}

void ModelLoader::CacheModel(const ResourceLocation& location,
                             const std::unique_ptr<BlockModel>& model) {
  // Check if the model is already cached
  const auto& it = cache_.find(location);
  if (it != cache_.find(location)) {
    LOG_DEBUG("Attempted to cache model that is already cached");
  } else {
    // Make copy
    cache_[location] = std::make_unique<BlockModel>(*model);
  }
}

void ModelLoader::ProcessModelDisplay(std::unique_ptr<BlockModel>& model,
                                      const json& displays) {
  for (const auto& [display_pos, info] : displays.items()) {
    BlockDisplay display;
    if (info.contains("rotation")) {
      auto vals = info["rotation"].get<std::array<float, 3>>();
      for (int i = 0; i < 3; i++) {
        display.rotation_[i] = vals[i];
      }
    }

    if (info.contains("translation")) {
      auto vals = info["translation"].get<std::array<float, 3>>();
      for (int i = 0; i < 3; i++) {
        display.translation_[i] = vals[i];
      }
    }

    if (info.contains("scale")) {
      auto vals = info["scale"].get<std::array<float, 3>>();
      for (int i = 0; i < 3; i++) {
        display.scale_[i] = vals[i];
      }
    }

    if (display_pos == "thirdperson_righthand") {
      display.position_ = DisplayPosition::thirdperson_righthand;
    } else if (display_pos == "thirdperson_lefthand") {
      display.position_ = DisplayPosition::thirdperson_lefthand;
    } else if (display_pos == "firstperson_righthand") {
      display.position_ = DisplayPosition::firstperson_righthand;
    } else if (display_pos == "firstperson_lefthand") {
      display.position_ = DisplayPosition::firstperson_lefthand;
    } else if (display_pos == "gui") {
      display.position_ = DisplayPosition::gui;
    } else if (display_pos == "head") {
      display.position_ = DisplayPosition::head;
    } else if (display_pos == "ground") {
      display.position_ = DisplayPosition::ground;
    } else if (display_pos == "fixed") {
      display.position_ = DisplayPosition::fixed;
    } else {
      LOG_WARN("Unknown display position: {}", display_pos);
    }

    display.initialized_ = true;
    model->AddDisplay(display, display.position_);
  }
}

void ModelLoader::UpdateModelElements(std::unique_ptr<BlockModel>& model,
                                      const json& elements) {
  for (const auto& element : elements) {
    Cuboid cuboid;

    if (element.contains("from")) {
      auto vals = element["from"].get<std::array<float, 3>>();
      for (int i = 0; i < 3; i++) {
        cuboid.from_[i] = vals[i] / 16.0f;
      }
    }

    if (element.contains("to")) {
      auto vals = element["to"].get<std::array<float, 3>>();
      for (int i = 0; i < 3; i++) {
        cuboid.to_[i] = vals[i] / 16.0f;
      }
    }

    if (element.contains("faces")) {
      ProcessSingleCubeFaces(cuboid, element["faces"]);
    }

    if (element.contains("rotation")) {
      cuboid.rotation_ = GetRotationalData(element["rotation"]);
    }

    if (element.contains("__comment")) {
      cuboid.comments_ = element["__comment"].get<std::string>();
    }

    if (element.contains("shade")) {
      cuboid.shade_ = element["shade"].get<bool>();
    }
    model->AddElement(cuboid);
  }
}

void ModelLoader::ProcessCuboidTexture(std::unique_ptr<BlockModel>& model,
                                       const json& textures) {
  for (const auto& [name, path] : textures.items()) {
    std::string texture_def = path.get<std::string>();

    size_t idx = texture_def.find(':');

    std::string texture_location;
    std::string texture_namespace;

    if (idx != std::string::npos) {
      texture_location = texture_def.substr(idx + 1);
      texture_namespace = texture_def.substr(0, idx);
    } else {
      texture_location = texture_def;
      texture_namespace = kDefaultNamespace;
    }

    bool is_variable = texture_location[0] == '#';
    if (is_variable) {
      model->texture_variable_[name] = texture_location;
    } else {
      model->texture_variable_[name] =
          texture_namespace + ":" + texture_location;
    }
  }
}

CuboidRotationInfo ModelLoader::GetRotationalData(const json& rotation_data) {
  CuboidRotationInfo rotation_info;

  if (rotation_data.contains("origin")) {
    auto vals = rotation_data["origin"].get<std::array<float, 3>>();

    for (int i = 0; i < 3; i++) {
      rotation_info.origin_[i] = vals[i] / 16;
    }
  }

  if (rotation_data.contains("axis")) {
    char axis = rotation_data["axis"].get<std::string>()[0];

    if (axis == 'x') {
      rotation_info.axis_ = 0;
    } else if (axis == 'y') {
      rotation_info.axis_ = 1;
    } else if (axis == 'z') {
      rotation_info.axis_ = 2;
    }
  }

  if (rotation_data.contains("angle")) {
    rotation_info.angle_ = rotation_data["angle"].get<float>();
  }

  if (rotation_data.contains("rescale")) {
    rotation_info.rescale_ = rotation_data["rescale"].get<bool>();
  }

  rotation_info.initialized_ = true;

  return rotation_info;
}

void ModelLoader::ProcessSingleCubeFaces(Cuboid& cube, const json& faces) {
  for (const auto& [direction, face] : faces.items()) {
    BlockFace face_data;
    int face_idx = ConvertStringFaceToIndex(direction);

    if (face.contains("uv")) {
      auto arr = face["uv"].get<std::array<float, 4>>();

      // flip
      arr[1] = 16 - arr[1];
      arr[3] = 16 - arr[3];

      std::swap(arr[1], arr[3]);

      for (int i = 0; i < 4; i++) {
        face_data.uv_[i] = arr[i];
      }
    }

    if (face.contains("texture")) {
      std::string texture_def = face["texture"].get<std::string>();

      size_t idx = texture_def.find(':');

      std::string texture_location;
      std::string texture_namespace;

      if (idx != std::string::npos) {
        texture_location = texture_def.substr(idx + 1);
        texture_namespace = texture_def.substr(0, idx);
      } else {
        texture_location = texture_def;
        texture_namespace = kDefaultNamespace;
      }

      bool isVariable = texture_location[0] == '#';

      if (isVariable) {
        face_data.reference_texture_ = texture_location;
      } else {
        face_data.reference_texture_ =
            texture_namespace + ":" + texture_location;
      }
    }

    if (face.contains("cullface")) {
      face_data.cull_face_ =
          ConvertStringFaceToIndex(face["cullface"].get<std::string>());
    }

    if (face.contains("tintindex")) {
      face_data.tint_index_ = face["tintindex"].get<int>();
    }

    if (face.contains("rotation")) {
      // Todo(hiheyok): change to int later
      face_data.rotation_ = face["rotation"].get<uint32_t>();
    }
    cube.EditFace(face_idx, face_data);
  }
}

int ModelLoader::ConvertStringFaceToIndex(const std::string& str) {
  if (str == "down") {
    return kDownDirection;
  } else if (str == "up") {
    return kUpDirection;
  } else if (str == "north") {
    return kNorthDirection;
  } else if (str == "south") {
    return kSouthDirection;
  } else if (str == "west") {
    return kWestDirection;
  } else if (str == "east") {
    return kEastDirection;
  } else {
    LOG_WARN("Unknown direction: {}", str);
    return 0;
  }
}

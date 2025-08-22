#include "Assets/Types/Models/Managers/ModelLoader.h"

#include <array>

#include "Core/GameContext/GameContext.h"
#include "Core/IO/FileUtils.h"
#include "Core/Position/Direction.h"
#include "Core/Registry/ResourceLocation.h"
#include "Utils/LogUtils.h"

using namespace model;

using glm::vec2;
using glm::vec3;
using std::array;
using std::nullopt;
using std::optional;
using std::reverse;
using std::string;
using std::vector;

decltype(ModelLoader::kStringDisplayMap) ModelLoader::kStringDisplayMap = {
    {"thirdperson_righthand", DisplayPosition::kThirdPersonRighthand},
    {"thirdperson_lefthand", DisplayPosition::kThirdPersonLefthand},
    {"firstperson_righthand", DisplayPosition::kFirstPersonRighthand},
    {"firstperson_lefthand", DisplayPosition::kFirstPersonLefthand},
    {"gui", DisplayPosition::kGui},
    {"head", DisplayPosition::kHead},
    {"ground", DisplayPosition::kGround},
    {"fixed", DisplayPosition::kFixed},
    {"on_shelf", DisplayPosition::kOnShelf}};

decltype(ModelLoader::kStringDirectionMap) ModelLoader::kStringDirectionMap = {
    {"down", kDownDirection},   {"up", kUpDirection},
    {"north", kNorthDirection}, {"south", kSouthDirection},
    {"west", kWestDirection},   {"east", kEastDirection}};

ModelLoader::ModelLoader(GameContext& context) : context_{context} {}
ModelLoader::~ModelLoader() = default;

optional<ModelData> ModelLoader::GetModel(const ResourceLocation& location) {
  // Check if it is in the cache
  const auto& it = cache_.find(location);
  if (it != cache_.end()) {
    return it->second;
  }

  std::string filepath = location.GetPath() + ".json";

  // Loads up the json def
  vector<char> data = FileUtils::ReadFileToBuffer(context_, filepath);
  if (data.empty()) {
    return nullopt;
  }

  json manifest;

  try {
    manifest = json::parse(data);
  } catch (const json::exception& e) {
    LOG_ERROR("Failed to parse json: {}", e.what());
    return nullopt;
  }

  // Check for parent
  ModelData model;
  if (manifest.contains("parent")) {
    const string& parent_resource = manifest["parent"].get<string>();
    ResourceLocation parent{parent_resource, "models/"};
    auto parent_model = GetModel(parent);
    if (parent_model.has_value()) {
      model = parent_model.value();
    }
  }

  if (manifest.contains("textures")) {
    ProcessTexture(model, manifest["textures"]);
  }

  if (manifest.contains("ambientocclusion")) {
    model.ambient_occlusion_ = manifest["ambientocclusion"].get<bool>();
  }

  if (manifest.contains("display")) {
    ProcessDisplay(model, manifest["display"]);
  }

  if (manifest.contains("elements")) {
    // Elements in the current model overrides the parent
    model.elements_.clear();
    ProcessElement(model, manifest["elements"]);
  }

  if (manifest.contains("gui_light")) {
    const std::string& type = manifest["gui_light"].get<string>();
    if (type == "front") {
      model.gui_light_ = GuiLight::kFront;
    } else if (type == "side") {
      model.gui_light_ = GuiLight::kSide;
    } else {
      LOG_WARN("Unknown gui light option: {}", type);
    }
  }

  reverse(model.elements_.begin(), model.elements_.end());
  cache_.insert({location, model});
  return model;
}

void ModelLoader::ProcessTexture(ModelData& model, const json& data) {
  for (const auto& [key, val] : data.items()) {
    model.texture_variables_.emplace_back(key, val.get<string>());
  }
}

void ModelLoader::ProcessDisplay(ModelData& model, const json& data) {
  for (const auto& [position, transformations] : data.items()) {
    vec3 rotation{0.0f, 0.0f, 0.0f};
    vec3 translation{0.0f, 0.0f, 0.0f};
    vec3 scale{1.0f, 1.0f, 1.0f};

    if (transformations.contains("rotation")) {
      auto vals = transformations["rotation"].get<array<float, 3>>();
      rotation.x = vals[0];
      rotation.y = vals[1];
      rotation.z = vals[2];
    }

    if (transformations.contains("translation")) {
      auto vals = transformations["translation"].get<array<float, 3>>();
      translation.x = vals[0];
      translation.y = vals[1];
      translation.z = vals[2];
    }

    if (transformations.contains("scale")) {
      auto vals = transformations["scale"].get<array<float, 3>>();
      scale.x = vals[0];
      scale.y = vals[1];
      scale.z = vals[2];
    }

    Transformation transform;
    transform.rotation_ = rotation;
    transform.scale_ = scale;
    transform.translation_ = translation;

    const auto& it = kStringDisplayMap.find(position);

    if (it == kStringDisplayMap.end()) {
      LOG_WARN("Skipping display position. Unknown display position: {}",
               position);
      continue;
    }

    model.displays_.emplace_back(it->second, transform);
  }
}

void ModelLoader::ProcessElement(ModelData& model, const json& data) {
  for (const auto& element : data) {
    Element model_element;
    // Used to normalize these coordinates

    if (element.contains("from")) {
      auto vals = element["from"].get<array<float, 3>>();
      model_element.from_.x = vals[0] / kBlockLength;
      model_element.from_.y = vals[1] / kBlockLength;
      model_element.from_.z = vals[2] / kBlockLength;
    }

    if (element.contains("to")) {
      auto vals = element["to"].get<array<float, 3>>();
      model_element.to_.x = vals[0] / kBlockLength;
      model_element.to_.y = vals[1] / kBlockLength;
      model_element.to_.z = vals[2] / kBlockLength;
    }

    if (element.contains("rotation")) {
      const json& rot_data = element["rotation"];
      vec3 origin{0.0f, 0.0f, 0.0f};
      char axis = '\0';
      float angle = 0.0f;
      bool rescale = false;

      if (rot_data.contains("origin")) {
        auto vals = rot_data["origin"].get<array<float, 3>>();
        origin.x = vals[0] / kBlockLength;
        origin.y = vals[1] / kBlockLength;
        origin.z = vals[2] / kBlockLength;
      }

      if (rot_data.contains("axis")) {
        axis = rot_data["axis"].get<string>()[0];
      }

      if (rot_data.contains("angle")) {
        angle = rot_data["angle"].get<float>();
      }

      if (rot_data.contains("rescale")) {
        rescale = rot_data["rescale"].get<bool>();
      }

      Rotation rotation;
      rotation.angle_ = angle;
      rotation.axis_ = axis;
      rotation.origin_ = origin;
      rotation.rescale_ = rescale;

      model_element.rotation_ = rotation;
    }

    if (element.contains("shade")) {
      model_element.shade_ = element["shade"].get<bool>();
    }

    if (element.contains("light_emission")) {
      model_element.light_emission_ = element["light_emission"].get<int>();
    }

    if (element.contains("faces")) {
      ProcessFace(model_element, element["faces"]);
    }

    model.elements_.push_back(std::move(model_element));
  }
}

void ModelLoader::ProcessFace(Element& element, const json& data) {
  for (const auto& [direction, face_data] : data.items()) {
    const auto& it = kStringDirectionMap.find(direction);

    if (it == kStringDirectionMap.end()) {
      LOG_WARN("Skipping face, invallid direction: {}", direction);
      continue;
    }

    const size_t idx = it->second;

    Face face;

    if (face_data.contains("uv")) {
      auto vals = face_data["uv"].get<array<float, 4>>();
      face.uv_[0] = vals[0];
      face.uv_[1] = vals[1];  // Swap
      face.uv_[2] = vals[2];
      face.uv_[3] = vals[3];  // Swap
    } else {                  // Default values based on the element position
      // Find the face values
      vec2 from = element.from_;
      vec2 to = element.to_;

      face.uv_[0] = element.from_.x * kBlockLength;
      face.uv_[1] = element.from_.y * kBlockLength;
      face.uv_[2] = element.to_.x * kBlockLength;
      face.uv_[3] = element.to_.y * kBlockLength;
    }

    if (face_data.contains("texture")) {
      face.texture_ = face_data["texture"].get<string>();
    }

    if (face_data.contains("cullface")) {
      const string& cull = face_data["cullface"].get<string>();

      const auto& cull_it = kStringDirectionMap.find(cull);

      // If invalid direction, use default
      if (cull_it == kStringDirectionMap.end()) {
        face.cull_face_ = idx;  // Default
        LOG_WARN("Invalid cull face direction '{}', using default", cull);
      } else {
        face.cull_face_ = cull_it->second;
      }
    } else {
      face.cull_face_ = idx;  // Default
    }

    if (face_data.contains("rotation")) {
      face.texture_rotation_ = face_data["rotation"].get<int>();
    }

    if (face_data.contains("tintindex")) {
      face.tint_index_ = face_data["tintindex"].get<int>();
    }

    element.faces_[idx] = face;
  }
}

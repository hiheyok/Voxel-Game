#include "Assets/Types/Models/Managers/ModelManager.h"

#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Assets/AssetHandle.h"
#include "Assets/AssetManager.h"
#include "Assets/Types/Models/Data/ModelData.h"
#include "Assets/Types/Models/Data/RenderableModel.h"
#include "Assets/Types/Models/Managers/ModelLoader.h"
#include "Assets/Types/Texture/TextureAtlasSource.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Position/Direction.h"
#include "Core/Typenames.h"
#include "Utils/LogUtils.h"

using glm::mat4;
using glm::radians;
using glm::rotate;
using glm::vec3;
using glm::vec4;
using model::ModelLoader;
using std::array;
using std::make_unique;
using std::move;
using std::optional;
using std::string;
using std::swap;
using std::unique_ptr;
using std::vector;

ModelManager::ModelManager(GameContext& context)
    : context_{context}, loader_{make_unique<ModelLoader>(context)} {}
ModelManager::~ModelManager() = default;

AssetHandle<RenderableModel> ModelManager::LoadModel(
    const ResourceLocation& location) {
  // lazy
  atlas_ = context_.assets_->GetAtlasSource("blocks");
  // Verify that its in the cache
  const auto& it = models_.find(location);
  if (it != models_.end()) {
    return AssetHandle<RenderableModel>{it->second.get()};
  }

  optional<model::ModelData> data = loader_->GetModel(location);
  bool is_valid = data.has_value();

  if (!is_valid) {
    return AssetHandle<RenderableModel>{nullptr};
  }

  auto model = make_unique<RenderableModel>(context_, location.ToString());

  AssetHandle<RenderableModel> handle{model.get()};

  BakeModel(*model, data.value());
  models_.insert({location, move(model)});
  return handle;
}

void ModelManager::BakeModel(RenderableModel& model,
                             const model::ModelData& data) {
  // Use for getting the UVs for the model

  model.ambient_occlusion_ = data.ambient_occlusion_;
  BakeElement(model, data);
}

void ModelManager::BakeElement(RenderableModel& model,
                               const model::ModelData& data) {
  auto mapping = DecodeModelTextures(data);
  model.elements.reserve(data.elements_.size());
  for (const auto& element : data.elements_) {
    baked_model::Element baked_element;
    // Work on vertices
    vec3 from = element.from_;
    vec3 to = element.to_;

    baked_element.from_ = from;
    baked_element.to_ = to;
    baked_element.min_light_emission_ = element.light_emission_;
    baked_element.shade_ = element.shade_;

    // Iterate through the 8 corners
    for (int i = 0; i < 8; ++i) {
      if (i & 0b001) {
        baked_element.corners_[i].x = to.x;
      } else {
        baked_element.corners_[i].x = from.x;
      }

      if (i & 0b010) {
        baked_element.corners_[i].y = to.y;
      } else {
        baked_element.corners_[i].y = from.y;
      }

      if (i & 0b100) {
        baked_element.corners_[i].z = to.z;
      } else {
        baked_element.corners_[i].z = from.z;
      }
    }

    if (element.rotation_.has_value()) {
      const model::Rotation& rotation = element.rotation_.value();

      // Rotate the 8 models
      // Get rotational matrix
      mat4 rotation_matrix{1.0f};
      vec3 rotation_axis{0.0f, 0.0f, 0.0f};

      if (rotation.axis_ == 'x') {
        rotation_axis.x = 1.0f;
      } else if (rotation.axis_ == 'y') {
        rotation_axis.y = 1.0f;
      } else if (rotation.axis_ == 'z') {
        rotation_axis.z = 1.0f;
      } else {
        LOG_WARN("Invalid rotational axis: {}", rotation.axis_);
      }

      rotation_matrix =
          rotate(rotation_matrix, radians(rotation.angle_), rotation_axis);
      // Apply offsets

      vec3 offset = rotation.origin_;
      for (int i = 0; i < 8; ++i) {
        baked_element.corners_[i] -= offset;
      }

      for (int i = 0; i < 8; ++i) {
        vec4 rotated_vec =
            rotation_matrix * vec4(baked_element.corners_[i], 1.0f);
        baked_element.corners_[i].x = rotated_vec.x;
        baked_element.corners_[i].y = rotated_vec.y;
        baked_element.corners_[i].z = rotated_vec.z;
      }

      // Undo offsets
      for (int i = 0; i < 8; ++i) {
        baked_element.corners_[i] += offset;
      }
    }

    // Now work on the faces
    BakeFace(baked_element, element, mapping);
    model.elements.push_back(move(baked_element));
  }
}

void ModelManager::BakeFace(
    baked_model::Element& baked_element, const model::Element& element,
    const FastHashMap<std::string, ResourceLocation>& mapping) {
  for (int i = 0; i < 6; ++i) {
    const optional<model::Face>& face = element.faces_[i];
    if (!face.has_value()) {
      continue;
    }

    baked_model::Face baked_face;

    ResourceLocation location;

    if (face.value().texture_[0] == '#') {
      const auto& it = mapping.find(face.value().texture_);
      if (it == mapping.end()) {
        location = ResourceLocation{"blocks/null"};
      } else {
        location = it->second;
      }
    } else {
      location = ResourceLocation{face.value().texture_};
    }

    optional<TextureAtlasSource::Sprite> sprite = atlas_->GetSprite(location);
    const int atlas_x = atlas_->GetWidth();
    const int atlas_y = atlas_->GetHeight();

    if (sprite.has_value()) {
      vec4 uv;
      uv[0] = sprite->uv_beg_.x + face->uv_[0] / atlas_x;
      uv[1] = sprite->uv_beg_.y + face->uv_[1] / atlas_y;
      uv[2] = sprite->uv_beg_.x + face->uv_[2] / atlas_x;
      uv[3] = sprite->uv_beg_.y + face->uv_[3] / atlas_y;

      baked_face.uv_00_ = {uv[0], uv[1]};
      baked_face.uv_01_ = {uv[2], uv[1]};  // Swap
      baked_face.uv_10_ = {uv[0], uv[3]};  // Swap
      baked_face.uv_11_ = {uv[2], uv[3]};

      for (int r = 0; r < face->texture_rotation_; r += 90) {
        swap(baked_face.uv_00_, baked_face.uv_01_);
        swap(baked_face.uv_00_, baked_face.uv_10_);
        swap(baked_face.uv_10_, baked_face.uv_11_);
      }

      if ((i >> 1) == kZAxis) {
        swap(baked_face.uv_10_, baked_face.uv_01_);
      }

      baked_face.partial_trans_ = sprite->partial_trans_;
      baked_face.full_trans_ = sprite->full_trans_;
    }

    baked_face.cull_face_ = face.value().cull_face_;
    baked_face.tint_index_ = face.value().tint_index_;

    baked_element.faces_[i] = move(baked_face);
  }
}

FastHashMap<string, ResourceLocation> ModelManager::DecodeModelTextures(
    const model::ModelData& model) {
  FastHashMap<string, ResourceLocation> mapping;
  FastHashMap<string, string> original_mapping;

  for (const auto& [key, val] : model.texture_variables_) {
    original_mapping.insert({"#" + key, val});
  }

  for (const auto& [key, val] : original_mapping) {
    if (mapping.contains(key)) {
      continue;
    }

    FastHashSet<string> used;
    vector<string> variables;

    variables.push_back(key);

    string next_val = val;
    string next_key = key;
    while (next_val[0] == '#') {
      const auto& next = original_mapping.find(next_val);
      if (next == original_mapping.end()) {
        break;
      }

      next_key = next->first;
      next_val = next->second;
      // Detect cycles
      if (used.contains(next_key)) {
        break;
      }

      const auto& it = mapping.find(next_key);

      if (it != mapping.end()) {
        next_val = it->second.ToString();
        break;  // Early stop;
      }

      used.insert(next_key);
      variables.push_back(next_key);
    }

    if (next_val[0] == '#') {
      continue;  // Skipping; unable to find location
    }

    ResourceLocation location{next_val};

    for (const auto& key : variables) {
      mapping.insert({key, location});
    }
  }

  return mapping;
}

#include "Assets/AssetManager.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

#include "Assets/AssetHandle.h"
#include "Assets/Types/EntityModel.h"
#include "Assets/Types/Models/Managers/BlockModelManager.h"
#include "Assets/Types/ShaderSource.h"
#include "Assets/Types/Texture/Texture2DSource.h"
#include "Assets/Types/Texture/TextureAtlasSource.h"
#include "Core/GameContext/GameContext.h"
#include "Core/IO/FileUtils.h"
#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Utils/LogUtils.h"

using glm::vec2;
using glm::vec3;
using std::ifstream;
using std::make_unique;
using std::move;
using std::string;
using std::unique_ptr;
using std::vector;

AssetManager::AssetManager(GameContext& context)
    : context_{context},
      block_models_{make_unique<BlockModelManager>(context)} {}
AssetManager::~AssetManager() = default;

AssetHandle<ShaderSource> AssetManager::GetShaderSource(string key) {
  return GetAsset<ShaderSource>(key);
}

vector<AssetHandle<ShaderSource>> AssetManager::GetAllShaderSource() {
  return GetAllAsset<ShaderSource>();
}

AssetHandle<Texture2DSource> AssetManager::GetTexture2DSource(string key) {
  return GetAsset<Texture2DSource>(key);
}

vector<AssetHandle<Texture2DSource>> AssetManager::GetAllTexture2DSource() {
  return GetAllAsset<Texture2DSource>();
}

AssetHandle<TextureAtlasSource> AssetManager::GetAtlasSource(string key) {
  return GetAsset<TextureAtlasSource>(key);
}

vector<AssetHandle<TextureAtlasSource>> AssetManager::GetAllAtlasSource() {
  return GetAllAsset<TextureAtlasSource>();
}

AssetHandle<EntityModel> AssetManager::GetEntityModel(string key) {
  return GetAsset<EntityModel>(key);
}

const BlockModelManager& AssetManager::GetBlockModelManager() const {
  return *block_models_;
}

void AssetManager::Initialize() {
  DiscoverShaders();
  DiscoverTextures();
  DiscoverEntityModels();
}

void AssetManager::Load() {
  for (auto& [key, asset] : GetCache<ShaderSource>()) {
    asset->Load();
  }

  for (auto& [key, asset] : GetCache<Texture2DSource>()) {
    asset->Load();
  }

  for (auto& [key, asset] : GetCache<TextureAtlasSource>()) {
    asset->Load();
  }

  block_models_->Load();
}

void AssetManager::DiscoverShaders() {
  using json = nlohmann::json;

  const string manifest_path = "assets/manifest/shaders.json";

  vector<char> json_data = FileUtils::ReadFileToBuffer(context_, manifest_path);

  json manifest = json::parse(json_data);
  try {
    manifest = json::parse(json_data);
  } catch (const json::parse_error& e) {
    LOG_ERROR("Failed to parse shader manifest '{}': {}", manifest_path,
              e.what());
    return;
  }

  if (!manifest.contains("shaders") || !manifest["shaders"].is_array()) {
    LOG_WARN("Shader manifest '{}' is missing a 'shaders' array",
             manifest_path);
    return;
  }

  for (const auto& shader_def : manifest["shaders"]) {
    if (!shader_def.contains("name") || !shader_def.contains("type")) {
      LOG_WARN(
          "Skipping shader definition in '{}' due to missing 'name' or 'type'.",
          manifest_path);
      continue;
    }

    string name = shader_def["name"].get<string>();
    string type = shader_def["type"].get<string>();

    unique_ptr<ShaderSource> source;

    try {
      if (type == "graphics") {
        if (!shader_def.contains("vertex") ||
            !shader_def.contains("fragment")) {
          LOG_WARN(
              "Skipping shader definition in '{}' due to missing 'name' or "
              "'type'.",
              manifest_path);
          continue;
        }

        string geom_path = shader_def.value(
            "geometry", "");  // .value() provides a default if key is missing.

        source = ShaderSource::CreateShader(
            context_, name, shader_def["vertex"].get<string>(),
            shader_def["fragment"].get<string>(), geom_path);

      } else if (type == "compute") {
        if (!shader_def.contains("compute")) {
          LOG_WARN(
              "Skipping shader definition in '{}' due to missing 'name' or "
              "'type'.",
              manifest_path);
          continue;
        }

        source = ShaderSource::CreateComputeShader(
            context_, name, shader_def["compute"].get<string>());
      } else {
        LOG_WARN("Skipping shader '{}' with unknown type: {}", name, type);
        continue;
      }

      PutAsset(name, move(source));

    } catch (const json::type_error& e) {
      LOG_WARN("Type error in shader definition for '{}': {}", name, e.what());
    }
  }
}

void AssetManager::DiscoverTextures() {
  CreateAsset<Texture2DSource>("ascii_font",
                               "assets/minecraft/textures/font/ascii.png");
  CreateAsset<Texture2DSource>("light_map",
                               "assets/minecraft/textures/colormap/light.png");
  CreateAsset<TextureAtlasSource>("blocks",
                                  "assets/minecraft/atlases/blocks.json");
  CreateAsset<TextureAtlasSource>("gui", "assets/minecraft/atlases/gui.json");
  CreateAsset<TextureAtlasSource>("paintings",
                                  "assets/minecraft/atlases/paintings.json");
  CreateAsset<TextureAtlasSource>("particles",
                                  "assets/minecraft/atlases/particles.json");
  CreateAsset<TextureAtlasSource>("signs",
                                  "assets/minecraft/atlases/signs.json");
}

void AssetManager::DiscoverEntityModels() {
  using json = nlohmann::json;

  ifstream file("assets/EntityShape.json");

  json data = json::parse(file);

  for (auto& b : data.items()) {
    const string& model_name = b.key();

    unique_ptr<EntityModel> model = make_unique<EntityModel>(model_name);

    json::iterator d = b.value().begin();
    string val = d.value();
    string key = b.key();

    if (d.value().is_string()) {
      LOG_INFO("Entity: {} | Texture Loading: {}", key, val);
      const string& texture_id_ = d.value();

      CreateAsset<Texture2DSource>(model_name, texture_id_);
    }

    d++;

    vec3 hitboxSize(d.value().at(0), d.value().at(1), d.value().at(2));
    model->hitbox_ = hitboxSize;

    d++;

    for (auto& SubData : d.value().items()) {
      json::iterator it = SubData.value().begin();

      vec3 offset(it.value().at(0), it.value().at(1), it.value().at(2));

      it++;

      vec3 shapeSize(it.value().at(0), it.value().at(1), it.value().at(2));

      it++;

      Model::RectangularPrism* rect_model =
          model->AddRectangle(shapeSize, offset);

      for (auto& ShapeUV : it.value().items()) {
        json::iterator uv_iterator = ShapeUV.value().begin();

        vector<int> uvFaces = {};

        for (auto& uvFace : uv_iterator.value().items()) {
          int s = 0xFF;

          string texSide = uvFace.value();

          // TODO(hiheyok): refactor FRONT BACK LEFT RIGHT TOP BOTTOM to new
          // directions
          if (texSide == "FRONT") s = Directions<BlockPos>::kEast;
          if (texSide == "BACK") s = Directions<BlockPos>::kWest;
          if (texSide == "LEFT") s = Directions<BlockPos>::kNorth;
          if (texSide == "RIGHT") s = Directions<BlockPos>::kSouth;
          if (texSide == "TOP") s = Directions<BlockPos>::kUp;
          if (texSide == "BOTTOM") s = Directions<BlockPos>::kDown;

          uvFaces.push_back(s);
        }

        uv_iterator++;

        vec2 pts[2]{};

        int index = 0;

        for (auto& UV_Points :
             uv_iterator.value().items()) {  // iterate though uv points
          pts[1 - index] =
              vec2(static_cast<float>(UV_Points.value().at(0)),
                   1.f - static_cast<float>(UV_Points.value().at(1)));
          index++;
        }

        for (int& face : uvFaces) {
          rect_model->uv_map_[face].p0_ = pts[0];
          rect_model->uv_map_[face].p1_ = pts[1];
        }
      }
    }

    PutAsset(model_name, move(model));
  }
}

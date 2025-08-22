#include "Assets/AssetManager.h"

#include <nlohmann/json.hpp>

#include "Assets/Types/Models/Managers/BlockModelManager.h"
#include "Assets/Types/ShaderSource.h"
#include "Assets/Types/Texture/Texture2DSource.h"
#include "Assets/Types/Texture/TextureAtlasSource.h"
#include "Core/GameContext/GameContext.h"
#include "Core/IO/FileUtils.h"

using std::make_unique;

AssetManager::AssetManager(GameContext& context)
    : context_{context},
      block_models_{make_unique<BlockModelManager>(context)} {}
AssetManager::~AssetManager() = default;

AssetHandle<ShaderSource> AssetManager::GetShaderSource(std::string key) {
  return GetAsset<ShaderSource>(key);
}

std::vector<AssetHandle<ShaderSource>> AssetManager::GetAllShaderSource() {
  return GetAllAsset<ShaderSource>();
}

AssetHandle<Texture2DSource> AssetManager::GetTexture2DSource(std::string key) {
  return GetAsset<Texture2DSource>(key);
}

std::vector<AssetHandle<Texture2DSource>>
AssetManager::GetAllTexture2DSource() {
  return GetAllAsset<Texture2DSource>();
}

AssetHandle<TextureAtlasSource> AssetManager::GetAtlasSource(std::string key) {
  return GetAsset<TextureAtlasSource>(key);
}

std::vector<AssetHandle<TextureAtlasSource>> AssetManager::GetAllAtlasSource() {
  return GetAllAsset<TextureAtlasSource>();
}

const BlockModelManager& AssetManager::GetBlockModelManager() const {
  return *block_models_;
}

void AssetManager::Initialize() {
  DiscoverShaders();
  DiscoverTextures();
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

  const std::string manifest_path = "assets/minecraft/manifest/shaders.json";

  std::vector<char> json_data =
      FileUtils::ReadFileToBuffer(context_, manifest_path);

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

    std::string name = shader_def["name"].get<std::string>();
    std::string type = shader_def["type"].get<std::string>();

    std::unique_ptr<ShaderSource> source;

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

        std::string geom_path = shader_def.value(
            "geometry", "");  // .value() provides a default if key is missing.

        source = ShaderSource::CreateShader(
            context_, name, shader_def["vertex"].get<std::string>(),
            shader_def["fragment"].get<std::string>(), geom_path);

      } else if (type == "compute") {
        if (!shader_def.contains("compute")) {
          LOG_WARN(
              "Skipping shader definition in '{}' due to missing 'name' or "
              "'type'.",
              manifest_path);
          continue;
        }

        source = ShaderSource::CreateComputeShader(
            context_, name, shader_def["compute"].get<std::string>());
      } else {
        LOG_WARN("Skipping shader '{}' with unknown type: {}", name, type);
        continue;
      }

      PutAsset(name, std::move(source));

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
  CreateAsset<Texture2DSource>("widgets",
                               "assets/minecraft/textures/gui/widgets.png");

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

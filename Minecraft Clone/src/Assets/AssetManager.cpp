#include "Assets/AssetManager.h"

#include "Assets/Types/ShaderSource.h"

AssetManager::AssetManager(GameContext& context) : context_{context} {}
AssetManager::~AssetManager() = default;

AssetHandle<ShaderSource> AssetManager::GetShaderSource(std::string key) {
  return GetAsset<ShaderSource>(key);
}

void AssetManager::Initialize() {
  CreateAsset<ShaderSource>("world_render", "assets/shaders/vert.glsl",
                            "assets/shaders/frag.glsl");
  CreateAsset<ShaderSource>("entity_render",
                            "assets/shaders/Entity/MultiEntityVert.glsl",
                            "assets/shaders/Entity/MultiEntityFrag.glsl");
  CreateAsset<ShaderSource>("gui_render", "assets/shaders/GUI/GUIVert.glsl",
                            "assets/shaders/GUI/GUIFrag.glsl");
  CreateAsset<ShaderSource>("fb_render", "assets/shaders/screen/vert.glsl",
                            "assets/shaders/screen/frag.glsl");
}

void AssetManager::Load() {
  for (auto& [key, asset] : shader_cache_) {
    asset->Load();
  }
}
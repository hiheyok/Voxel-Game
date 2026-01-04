#include "RenderEngine/RenderResources/RenderResourceManager.h"

#include "Assets/AssetManager.h"
#include "Assets/Types/ShaderSource.h"
#include "Assets/Types/Texture/Texture2DSource.h"
#include "Assets/Types/Texture/TextureAtlasSource.h"
#include "Core/GameContext/GameContext.h"
#include "RenderEngine/OpenGL/Shader/ComputeShader.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/RenderResources/Manager/ItemIconManager.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2D.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureAtlas.h"

using std::make_unique;
using std::string;
using std::vector;

RenderResourceManager::RenderResourceManager(GameContext& context)
    : context_{context} {}
RenderResourceManager::~RenderResourceManager() = default;

void RenderResourceManager::Initialize() {
  FindShaders();
  FindTextures();
}

void RenderResourceManager::Load() {
  LoadShaders();
  LoadTextures();

  item_icon_manager_ = make_unique<ItemIconManager>(context_);
}

RenderHandle<Shader> RenderResourceManager::GetShader(const string& key) {
  return GetResource<Shader>(key);
}

RenderHandle<ComputeShader> RenderResourceManager::GetComputeShader(
    const string& key) {
  return GetResource<ComputeShader>(key);
}

RenderHandle<Texture2DV2> RenderResourceManager::GetTexture2D(
    const string& key) {
  return GetResource<Texture2DV2>(key);
}

RenderHandle<TextureAtlas> RenderResourceManager::GetAtlas(const string& key) {
  return GetResource<TextureAtlas>(key);
}

ItemIconManager& RenderResourceManager::GetItemIconManager() {
  return *item_icon_manager_;
}

void RenderResourceManager::LoadShaders() {
  auto& shader_cache = GetCache<Shader>();
  auto& compute_cache = GetCache<ComputeShader>();

  for (auto& [key, shader] : shader_cache) {
    shader->Load();
  }

  for (auto& [key, compute] : compute_cache) {
    compute->Load();
  }
}

void RenderResourceManager::LoadTextures() {
  for (auto& [key, resource] : GetCache<Texture2DV2>()) {
    resource->Load();
  }

  for (auto& [key, resource] : GetCache<TextureAtlas>()) {
    resource->Load();
  }
}

void RenderResourceManager::FindShaders() {
  AssetManager& assets = *context_.assets_;
  vector<AssetHandle<ShaderSource>> sources = assets.GetAllShaderSource();

  for (auto& source : sources) {
    if (source->GetType() == ShaderSource::ShaderType::kShader) {
      CreateResource<Shader>(source->GetKey(), source);
    } else {
      CreateResource<ComputeShader>(source->GetKey(), source);
    }
  }
}

void RenderResourceManager::FindTextures() {
  AssetManager& assets = *context_.assets_;
  vector<AssetHandle<Texture2DSource>> tex_2d = assets.GetAllTexture2DSource();

  vector<AssetHandle<TextureAtlasSource>> atlas = assets.GetAllAtlasSource();

  for (auto& source : tex_2d) {
    CreateResource<Texture2DV2>(source->GetKey(), source);
  }

  for (auto& source : atlas) {
    CreateResource<TextureAtlas>(source->GetKey(), source);
  }
}

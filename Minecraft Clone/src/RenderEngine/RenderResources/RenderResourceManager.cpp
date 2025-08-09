#include "RenderEngine/RenderResources/RenderResourceManager.h"

#include "Assets/AssetManager.h"
#include "Assets/Types/ShaderSource.h"
#include "Core/GameContext/GameContext.h"
#include "RenderEngine/OpenGL/Shader/ComputeShader.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"

RenderResourceManager::RenderResourceManager(GameContext& context)
    : context_{context} {}
RenderResourceManager::~RenderResourceManager() = default;

void RenderResourceManager::Initialize() { FindShaders(); }

void RenderResourceManager::Load() { LoadShaders(); }

RenderHandle<Shader> RenderResourceManager::GetShader(const std::string& key) {
  return GetResource<Shader>(key);
}

RenderHandle<ComputeShader> RenderResourceManager::GetComputeShader(
    const std::string& key) {
  return GetResource<ComputeShader>(key);
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

void RenderResourceManager::FindShaders() {
  AssetManager& assets = *context_.assets_;
  std::vector<AssetHandle<ShaderSource>> sources = assets.GetAllShaderSource();

  for (auto& source : sources) {
    if (source->GetType() == ShaderSource::ShaderType::kShader) {
      CreateResource<Shader>(source->GetKey(), source);
    } else {
      CreateResource<ComputeShader>(source->GetKey(), source);
    }
  }
}

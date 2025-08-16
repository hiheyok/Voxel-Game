#pragma once

#include <memory>
#include <string>
#include <type_traits>

#include "Core/Typenames.h"
#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/RenderResources/RenderResource.h"

class GameContext;

class Shader;
class ComputeShader;
class Texture2DV2;
class TextureAtlasV2;

template <typename T>
concept IsResource = std::is_base_of_v<RenderResource, std::decay_t<T>>;

class RenderResourceManager {
 public:
  explicit RenderResourceManager(GameContext&);
  ~RenderResourceManager();

  // Task with finding all of the resources first
  void Initialize();

  // This will load data from assets to GPU
  void Load();

  RenderHandle<Shader> GetShader(const std::string& key);
  RenderHandle<ComputeShader> GetComputeShader(const std::string& key);
  RenderHandle<Texture2DV2> GetTexture2D(const std::string& key);
  RenderHandle<TextureAtlasV2> GetAtlas(const std::string& key);

 private:
  void LoadShaders();
  void LoadTextures();

  void FindShaders();
  void FindTextures();

  template <IsResource ResourceType, typename... Args>
  void CreateResource(std::string key, Args&&... args);

  template <IsResource ResourceType>
  void PutResource(std::string key, std::unique_ptr<ResourceType> asset);

  template <IsResource ResourceType>
  RenderHandle<ResourceType> GetResource(std::string key);

  template <IsResource ResourceType>
  FastHashMap<std::string, std::unique_ptr<ResourceType>>& GetCache() noexcept;

  FastHashMap<std::string, std::unique_ptr<Shader>> shaders_cache_;
  FastHashMap<std::string, std::unique_ptr<ComputeShader>> compute_cache_;
  FastHashMap<std::string, std::unique_ptr<Texture2DV2>> texture2d_;
  FastHashMap<std::string, std::unique_ptr<TextureAtlasV2>> atlas_;

  GameContext& context_;
};

template <IsResource ResourceType>
FastHashMap<std::string, std::unique_ptr<ResourceType>>&
RenderResourceManager::GetCache() noexcept {
  using Type = std::decay_t<ResourceType>;

  if constexpr (std::is_same_v<Shader, Type>) {
    return shaders_cache_;
  } else if constexpr (std::is_same_v<ComputeShader, Type>) {
    return compute_cache_;
  } else if constexpr (std::is_same_v<Texture2DV2, Type>) {
    return texture2d_;
  } else if constexpr (std::is_same_v<TextureAtlasV2, Type>) {
    return atlas_;
  } else {
    static_assert(
        std::is_void_v<Type>,
        "RenderResourceManager::GetCache: Unsupported asset type requested.");
  }
}

template <IsResource ResourceType, typename... Args>
void RenderResourceManager::CreateResource(std::string key, Args&&... args) {
  auto& cache = GetCache<ResourceType>();
  auto obj = std::make_unique<ResourceType>(context_, key,
                                            std::forward<Args>(args)...);
  cache.emplace(key, std::move(obj));
}

template <IsResource ResourceType>
void RenderResourceManager::PutResource(std::string key,
                                        std::unique_ptr<ResourceType> asset) {
  auto& cache = GetCache<ResourceType>();
  cache.emplace(key, std::move(asset));
}

template <IsResource ResourceType>
RenderHandle<ResourceType> RenderResourceManager::GetResource(std::string key) {
  auto& cache = GetCache<ResourceType>();
  auto it = cache.find(key);

  if (it != cache.end()) {
    return RenderHandle<ResourceType>{it->second.get()};
  } else {
    return RenderHandle<ResourceType>{};
  }
}

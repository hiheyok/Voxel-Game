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
class Texture2D;
class TextureAtlas;
class ItemIconManager;

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
  RenderHandle<Texture2D> GetTexture2D(const std::string& key);
  RenderHandle<TextureAtlas> GetAtlas(const std::string& key);

  ItemIconManager& GetItemIconManager();

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

  GameContext& context_;

  FastHashMap<std::string, std::unique_ptr<Shader>> shaders_cache_;
  FastHashMap<std::string, std::unique_ptr<ComputeShader>> compute_cache_;
  FastHashMap<std::string, std::unique_ptr<Texture2D>> texture2d_;
  FastHashMap<std::string, std::unique_ptr<TextureAtlas>> atlas_;

  std::unique_ptr<ItemIconManager> item_icon_manager_;
};

template <IsResource ResourceType>
FastHashMap<std::string, std::unique_ptr<ResourceType>>&
RenderResourceManager::GetCache() noexcept {
  using Type = std::decay_t<ResourceType>;

  if constexpr (std::is_same_v<Shader, Type>) {
    return shaders_cache_;
  } else if constexpr (std::is_same_v<ComputeShader, Type>) {
    return compute_cache_;
  } else if constexpr (std::is_same_v<Texture2D, Type>) {
    return texture2d_;
  } else if constexpr (std::is_same_v<TextureAtlas, Type>) {
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

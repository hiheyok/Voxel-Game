#pragma once

#include <memory>
#include <string>
#include <type_traits>

#include "Assets/Asset.h"
#include "Core/Typenames.h"

class GameContext;

template <typename T>
concept IsAsset = std::is_base_of_v<Asset, std::decay_t<T>>;

class ShaderSource;
class Texture2DSource;

class AssetManager {
 public:
  explicit AssetManager(GameContext&);
  ~AssetManager();

  AssetHandle<ShaderSource> GetShaderSource(std::string key);
  std::vector<AssetHandle<ShaderSource>> GetAllShaderSource();

  AssetHandle<Texture2DSource> GetTexture2DSource(std::string key);
  std::vector<AssetHandle<Texture2DSource>> GetTexture2DSource();

  // This will load in all of the keys
  void Initialize();

  // This will load in the data
  void Load();

 private:
  // Discovery functions
  void DiscoverShaders();
  void DiscoverTextures();

  template <IsAsset T, typename... Args>
  void CreateAsset(std::string key, Args&&... args);

  template <IsAsset T>
  void PutAsset(std::string key, std::unique_ptr<T> asset);

  template <IsAsset T>
  AssetHandle<T> GetAsset(std::string key);

  template <IsAsset T>
  std::vector<AssetHandle<T>> GetAllAsset();

  template <IsAsset T>
  FastHashMap<std::string, std::unique_ptr<T>>& GetCache() noexcept;

  GameContext& context_;

  FastHashMap<std::string, std::unique_ptr<ShaderSource>> shader_cache_;
  FastHashMap<std::string, std::unique_ptr<Texture2DSource>> texture2d_cache_;
};

template <IsAsset T>
FastHashMap<std::string, std::unique_ptr<T>>&
AssetManager::GetCache() noexcept {
  using Type = std::decay_t<T>;

  if constexpr (std::is_same_v<ShaderSource, Type>) {
    return shader_cache_;
  } else if constexpr (std::is_same_v<Texture2DSource, Type>) {
    return texture2d_cache_;
  } else {
    static_assert(std::is_void_v<Type>,
                  "AssetManager::GetCache: Unsupported asset type requested.");
  }
}

template <IsAsset T, typename... Args>
void AssetManager::CreateAsset(std::string key, Args&&... args) {
  auto& cache = GetCache<T>();
  auto obj = std::make_unique<T>(context_, key, std::forward<Args>(args)...);
  cache.emplace(key, std::move(obj));
}

template <IsAsset T>
void AssetManager::PutAsset(std::string key, std::unique_ptr<T> asset) {
  auto& cache = GetCache<T>();
  cache.emplace(key, std::move(asset));
}

template <IsAsset T>
AssetHandle<T> AssetManager::GetAsset(std::string key) {
  auto& cache = GetCache<T>();
  auto it = cache.find(key);

  if (it != cache.end()) {
    return AssetHandle<T>{it->second.get()};
  } else {
    return AssetHandle<T>{};
  }
}

template <IsAsset T>
std::vector<AssetHandle<T>> AssetManager::GetAllAsset() {
  auto& cache = GetCache<T>();
  std::vector<AssetHandle<T>> out;
  out.reserve(cache.size());
  for (auto& [key, asset] : cache) {
    out.emplace_back(asset.get());
  }
  return out;
}

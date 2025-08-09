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

class AssetManager {
 public:
  explicit AssetManager(GameContext&);
  ~AssetManager();

  AssetHandle<ShaderSource> GetShaderSource(std::string key);
  std::vector<AssetHandle<ShaderSource>> GetAllShaderSource();

  // This will load in all of the keys
  void Initialize();

  // This will load in the data
  void Load();

 private:
  // Discovery functions
  void DiscoverShaders();

  template <IsAsset AssetType, typename... Args>
  void CreateAsset(std::string key, Args&&... args);

  template <IsAsset AssetType>
  void PutAsset(std::string key, std::unique_ptr<AssetType> asset);

  template <IsAsset AssetType>
  AssetHandle<AssetType> GetAsset(std::string key);

  template <IsAsset AssetType>
  std::vector<AssetHandle<AssetType>> GetAllAsset();

  template <IsAsset AssetType>
  FastHashMap<std::string, std::unique_ptr<AssetType>>& GetCache() noexcept;

  GameContext& context_;

  FastHashMap<std::string, std::unique_ptr<ShaderSource>> shader_cache_;
};

template <IsAsset AssetType>
FastHashMap<std::string, std::unique_ptr<AssetType>>&
AssetManager::GetCache() noexcept {
  using Type = std::decay_t<AssetType>;

  if constexpr (std::is_same_v<ShaderSource, Type>) {
    return shader_cache_;
  } else {
    static_assert(std::is_void_v<Type>,
                  "AssetManager::GetCache: Unsupported asset type requested.");
  }
}

template <IsAsset AssetType, typename... Args>
void AssetManager::CreateAsset(std::string key, Args&&... args) {
  auto& cache = GetCache<AssetType>();
  auto obj =
      std::make_unique<AssetType>(context_, key, std::forward<Args>(args)...);
  cache.emplace(key, std::move(obj));
}

template <IsAsset AssetType>
void AssetManager::PutAsset(std::string key, std::unique_ptr<AssetType> asset) {
  auto& cache = GetCache<AssetType>();
  cache.emplace(key, std::move(asset));
}

template <IsAsset AssetType>
AssetHandle<AssetType> AssetManager::GetAsset(std::string key) {
  auto& cache = GetCache<AssetType>();
  auto it = cache.find(key);

  if (it != cache.end()) {
    return AssetHandle<AssetType>{it->second.get()};
  } else {
    return AssetHandle<AssetType>{};
  }
}

template <IsAsset AssetType>
std::vector<AssetHandle<AssetType>> AssetManager::GetAllAsset() {
  auto& cache = GetCache<AssetType>();
  std::vector<AssetHandle<AssetType>> out;
  out.reserve(cache.size());
  for (auto& [key, asset] : cache) {
    out.emplace_back(asset.get());
  }
  return out;
}

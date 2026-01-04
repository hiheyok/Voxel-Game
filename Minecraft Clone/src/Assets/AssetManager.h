#pragma once

#include <memory>
#include <string>
#include <type_traits>

#include "Assets/Asset.h"
#include "Core/Typenames.h"
#include "Utils/Assert.h"

class GameContext;

template <typename T>
concept IsAsset = std::is_base_of_v<Asset, std::decay_t<T>>;

class ShaderSource;
class Texture2DSource;
class TextureAtlasSource;
class BlockModelManager;
class EntityModel;

class AssetManager {
 public:
  explicit AssetManager(GameContext&);
  ~AssetManager();

  AssetHandle<ShaderSource> GetShaderSource(const std::string& key);
  std::vector<AssetHandle<ShaderSource>> GetAllShaderSource();

  AssetHandle<Texture2DSource> GetTexture2DSource(const std::string& key);
  std::vector<AssetHandle<Texture2DSource>> GetAllTexture2DSource();

  AssetHandle<TextureAtlasSource> GetAtlasSource(const std::string& key);
  std::vector<AssetHandle<TextureAtlasSource>> GetAllAtlasSource();

  AssetHandle<EntityModel> GetEntityModel(const std::string& key);

  const BlockModelManager& GetBlockModelManager() const;

  // This will load in all of the keys
  void Initialize();

  // This will load in the data
  void Load();

 private:
  // Discovery functions
  void DiscoverShaders();
  void DiscoverTextures();
  void DiscoverEntityModels();

  template <IsAsset T, typename... Args>
  T& CreateAsset(const std::string& key, Args&&... args);

  template <IsAsset T>
  void PutAsset(const std::string& key, std::unique_ptr<T> asset);

  template <IsAsset T>
  AssetHandle<T> GetAsset(const std::string& key);

  template <IsAsset T>
  std::vector<AssetHandle<T>> GetAllAsset();

  template <IsAsset T>
  FastHashMap<std::string, std::unique_ptr<T>>& GetCache() noexcept;

  GameContext& context_;

  FastHashMap<std::string, std::unique_ptr<ShaderSource>> shader_cache_;
  FastHashMap<std::string, std::unique_ptr<Texture2DSource>> texture2d_cache_;
  FastHashMap<std::string, std::unique_ptr<TextureAtlasSource>> atlas_cache_;
  FastHashMap<std::string, std::unique_ptr<EntityModel>> entity_model_cache_;
  std::unique_ptr<BlockModelManager> block_models_;
};

template <IsAsset T>
FastHashMap<std::string, std::unique_ptr<T>>&
AssetManager::GetCache() noexcept {
  using Type = std::decay_t<T>;

  if constexpr (std::is_same_v<ShaderSource, Type>) {
    return shader_cache_;
  } else if constexpr (std::is_same_v<Texture2DSource, Type>) {
    return texture2d_cache_;
  } else if constexpr (std::is_same_v<TextureAtlasSource, Type>) {
    return atlas_cache_;
  } else if constexpr (std::is_same_v<EntityModel, Type>) {
    return entity_model_cache_;
  } else {
    static_assert(std::is_void_v<Type>,
                  "AssetManager::GetCache: Unsupported asset type requested.");
  }
}

template <IsAsset T, typename... Args>
T& AssetManager::CreateAsset(const std::string& key, Args&&... args) {
  auto& cache = GetCache<T>();
  auto obj = std::make_unique<T>(context_, key, std::forward<Args>(args)...);
  auto [it, success] = cache.emplace(key, std::move(obj));
  GAME_ASSERT(success, "Failed to insert assert");
  return *it->second;
}

template <IsAsset T>
void AssetManager::PutAsset(const std::string& key, std::unique_ptr<T> asset) {
  auto& cache = GetCache<T>();
  cache.emplace(key, std::move(asset));
}

template <IsAsset T>
AssetHandle<T> AssetManager::GetAsset(const std::string& key) {
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

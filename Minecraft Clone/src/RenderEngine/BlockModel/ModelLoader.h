// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "Core/Typenames.h"
#include "RenderEngine/BlockModel/BlockModels.h"

class GameContext;
class ResourceLocation;

class ModelLoader {
 public:
  explicit ModelLoader(GameContext&);
  ~ModelLoader();
  ModelLoader(const ModelLoader&) = delete;
  ModelLoader(ModelLoader&&) = delete;

  std::unique_ptr<BlockModel> GetModel(const ResourceLocation& location);

 private:
  using json = nlohmann::json;

  std::unique_ptr<BlockModel> GetModelRecursive(
      const ResourceLocation& location);

  void CacheModel(const ResourceLocation& location,
                  const std::unique_ptr<BlockModel>& model);
  void ProcessModelDisplay(std::unique_ptr<BlockModel>& model, json JsonData);
  void UpdateModelElements(std::unique_ptr<BlockModel>& model, json JsonData);
  void ProcessCuboidTexture(std::unique_ptr<BlockModel>& model, json JsonData);
  CuboidRotationInfo GetRotationalData(json JsonData);
  void ProcessSingleCubeFaces(Cuboid& cube, json JsonData);
  int ConvertStringFaceToIndex(const std::string& str);
  std::vector<float> GetJSONArrayValuesFloat(json JsonData);
  std::vector<int> GetJSONArrayValues(json JsonData);

  GameContext& game_context_;
  // Model cache to cache repeated parents
  FastHashMap<ResourceLocation, std::unique_ptr<BlockModel>> cache_;
  const std::string kDefaultNamespace = "minecraft";
};

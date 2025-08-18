// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "Core/Typenames.h"
#include "RenderEngine/Models/Block/BlockModels.h"

class GameContext;
class ResourceLocation;

class BlockModelLoader {
 public:
  explicit BlockModelLoader(GameContext&);
  ~BlockModelLoader();

  BlockModelLoader(const BlockModelLoader&) = delete;
  BlockModelLoader& operator=(const BlockModelLoader&) = delete;

  BlockModelLoader(BlockModelLoader&&) = delete;
  BlockModelLoader& operator=(BlockModelLoader&&) = delete;

  std::unique_ptr<BlockModel> GetModel(const ResourceLocation& location);

 private:
  using json = nlohmann::json;

  std::unique_ptr<BlockModel> GetModelRecursive(
      const ResourceLocation& location);

  void CacheModel(const ResourceLocation& location,
                  const std::unique_ptr<BlockModel>& model);
  void ProcessModelDisplay(std::unique_ptr<BlockModel>& model, const json&);
  void UpdateModelElements(std::unique_ptr<BlockModel>& model, const json&);
  void ProcessCuboidTexture(std::unique_ptr<BlockModel>& model, const json&);
  CuboidRotationInfo GetRotationalData(const json&);
  void ProcessSingleCubeFaces(Cuboid& cube, const json&);
  int ConvertStringFaceToIndex(const std::string& str);

  GameContext& context_;
  // Model cache to cache repeated parents
  FastHashMap<ResourceLocation, std::unique_ptr<BlockModel>> cache_;
  const std::string kDefaultNamespace = "minecraft";
};

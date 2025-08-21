#pragma once

#include <memory>
#include <string>

#include "Assets/AssetHandle.h"
#include "Core/Registry/ResourceLocation.h"
#include "Core/Typenames.h"

class GameContext;
class RenderableModel;

namespace baked_model {
struct Element;
}

namespace model {
class ModelLoader;
class ModelData;
struct Element;
};  // namespace model

class TextureAtlasSource;

// Base class giving shared functions used for loading and processing these
// models
class ModelManager {
 public:
  ModelManager(GameContext&);
  virtual ~ModelManager();

  ModelManager(const ModelManager&) = delete;
  ModelManager& operator=(const ModelManager&) = delete;

  ModelManager(ModelManager&&) = delete;
  ModelManager& operator=(ModelManager&&) = delete;

  AssetHandle<RenderableModel> LoadModel(const ResourceLocation& location);

 protected:
  GameContext& context_;

  FastHashMap<ResourceLocation, std::unique_ptr<RenderableModel>> models_;
  std::unique_ptr<model::ModelLoader> loader_;

 private:
  void BakeModel(RenderableModel& model, const model::ModelData& data);
  void BakeElement(RenderableModel& model, const model::ModelData& data);
  void BakeFace(baked_model::Element& model, const model::Element& data,
                const FastHashMap<std::string, ResourceLocation>& mapping);
  // Decodes all of the variables and return a mapping F: variable -> resource
  // location

  FastHashMap<std::string, ResourceLocation> DecodeModelTextures(
      const model::ModelData& model);

  AssetHandle<TextureAtlasSource> atlas_;
};

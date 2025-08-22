#pragma once

#include <nlohmann/json.hpp>
#include <optional>

#include "Assets/Types/Models/Data/ModelData.h"
#include "Core/Typenames.h"

class GameContext;
class ResourceLocation;

namespace model {

using nlohmann::json;

class ModelLoader {
 public:
  ModelLoader(GameContext&);
  ~ModelLoader();

  ModelLoader(const ModelLoader&) = delete;
  ModelLoader& operator=(const ModelLoader&) = delete;

  ModelLoader(ModelLoader&&) = delete;
  ModelLoader& operator=(ModelLoader&&) = delete;

  std::optional<ModelData> GetModel(const ResourceLocation& location);

  // Use for adding buildin models
  void AddDefaultModel(const ResourceLocation& location,
                       ModelData default_model);

 private:
  // Helper functions

  void ProcessTexture(ModelData& model, const json& data);
  void ProcessDisplay(ModelData& model, const json& data);
  void ProcessElement(ModelData& model, const json& data);
  void ProcessFace(Element& model, const json& data);
  
  static constexpr float kBlockLength = 16.0f;

  static const FastHashMap<std::string, DisplayPosition> kStringDisplayMap;
  static const FastHashMap<std::string, size_t> kStringDirectionMap;

  GameContext& context_;

  FastHashMap<ResourceLocation, ModelData> cache_;
};

}  // namespace model
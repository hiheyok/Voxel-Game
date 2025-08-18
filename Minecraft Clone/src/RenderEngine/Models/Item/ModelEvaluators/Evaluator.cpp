#include "RenderEngine/Models/Item/ModelEvaluators/Evaluator.h"

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/Models/Item/ModelEvaluators/BundleSelectedItemEvaluator.h"
#include "RenderEngine/Models/Item/ModelEvaluators/CompositeEvaluator.h"
#include "RenderEngine/Models/Item/ModelEvaluators/ConditionEvaluator.h"
#include "RenderEngine/Models/Item/ModelEvaluators/EmptyEvaluator.h"
#include "RenderEngine/Models/Item/ModelEvaluators/ModelEvaluator.h"
#include "RenderEngine/Models/Item/ModelEvaluators/RangeDispatchEvaluator.h"
#include "RenderEngine/Models/Item/ModelEvaluators/SelectEvaluator.h"
#include "RenderEngine/Models/Item/ModelEvaluators/SpecialEvaluator.h"
#include "Utils/LogUtils.h"

item_model::Evaluator::Evaluator(GameContext& context)
    : context_{context}, error_state_{false} {}
item_model::Evaluator::~Evaluator() = default;

std::unique_ptr<item_model::Evaluator> item_model::Evaluator::GetEvaluator(
    GameContext& context, const json& model_data) {
  if (!model_data.contains("type")) {
    LOG_STATIC_WARN(context.logger_,
                    "Error in parsing json model. Model has no type!");
    return nullptr;
  }

  const std::string& type = model_data["type"].get<std::string>();

  if (type == "minecraft:model") {
    return std::make_unique<ModelEvaluator>(context, model_data);
  } else if (type == "minecraft:composite") {
    return std::make_unique<CompositeEvaluator>(context, model_data);
  } else if (type == "minecraft:condition") {
    return std::make_unique<ConditionEvaluator>(context, model_data);
  } else if (type == "minecraft:select") {
    return std::make_unique<SelectEvaluator>(context, model_data);
  } else if (type == "minecraft:range_dispatch") {
    return std::make_unique<RangeDispatchEvaluator>(context, model_data);
  } else if (type == "minecraft:empty") {
    return std::make_unique<EmptyEvaluator>(context, model_data);
  } else if (type == "minecraft:bundle/selected_item") {
    return std::make_unique<BundleSelectedItemEvaluator>(context, model_data);
  } else if (type == "minecraft:special") {
    return std::make_unique<SpecialEvaluator>(context, model_data);
  } else {
    LOG_STATIC_WARN(context.logger_, "Unknown model type: {}", type);
  }
  return nullptr;
}
#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"

#include "Assets/Types/Models/Evaluators/Item/BundleSelectedItemEvaluator.h"
#include "Assets/Types/Models/Evaluators/Item/CompositeEvaluator.h"
#include "Assets/Types/Models/Evaluators/Item/ConditionEvaluator.h"
#include "Assets/Types/Models/Evaluators/Item/EmptyEvaluator.h"
#include "Assets/Types/Models/Evaluators/Item/ModelEvaluator.h"
#include "Assets/Types/Models/Evaluators/Item/RangeDispatchEvaluator.h"
#include "Assets/Types/Models/Evaluators/Item/SelectEvaluator.h"
#include "Assets/Types/Models/Evaluators/Item/SpecialEvaluator.h"
#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

using namespace item_model;
using std::make_unique;
using std::string;
using std::unique_ptr;

Evaluator::Evaluator(GameContext& context)
    : context_{context}, error_state_{false} {}
Evaluator::~Evaluator() = default;

unique_ptr<Evaluator> Evaluator::GetEvaluator(GameContext& context,
                                              const json& model_data) {
  if (!model_data.contains("type")) {
    LOG_STATIC_WARN(context.logger_,
                    "Error in parsing json model. Model has no type!");
    return nullptr;
  }

  const string& type = model_data["type"].get<string>();

  if (type == "minecraft:model") {
    return make_unique<ModelEvaluator>(context, model_data);
  } else if (type == "minecraft:composite") {
    return make_unique<CompositeEvaluator>(context, model_data);
  } else if (type == "minecraft:condition") {
    return make_unique<ConditionEvaluator>(context, model_data);
  } else if (type == "minecraft:select") {
    return make_unique<SelectEvaluator>(context, model_data);
  } else if (type == "minecraft:range_dispatch") {
    return make_unique<RangeDispatchEvaluator>(context, model_data);
  } else if (type == "minecraft:empty") {
    return make_unique<EmptyEvaluator>(context, model_data);
  } else if (type == "minecraft:bundle/selected_item") {
    return make_unique<BundleSelectedItemEvaluator>(context, model_data);
  } else if (type == "minecraft:special") {
    return make_unique<SpecialEvaluator>(context, model_data);
  } else {
    LOG_STATIC_WARN(context.logger_, "Unknown model type: {}", type);
  }
  return nullptr;
}
#include "RenderEngine/Models/Item/ModelEvaluators/ModelEvaluator.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

item_model::ModelEvaluator::ModelEvaluator(GameContext& context, const json&)
    : Evaluator{context} {}
item_model::ModelEvaluator::~ModelEvaluator() = default;

void item_model::ModelEvaluator::SetResourceLocation(
    ResourceLocation location) {
  location_ = location;
}

void item_model::ModelEvaluator::Evaluate(const EvaluatorContext& context,
                                          EvaluatedModels& models) const {
  models.push_back(location_);
}

void item_model::ModelEvaluator::Parse(const json& data) {
  // Ignore tints for now
  if (!data.contains("model")) {
    LOG_WARN("Incomplete model!");
    error_state_ = true;
    return;
  }

  location_ = data["model"].get<std::string>();
}

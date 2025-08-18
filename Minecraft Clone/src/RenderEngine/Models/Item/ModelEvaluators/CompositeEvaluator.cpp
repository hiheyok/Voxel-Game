#include "RenderEngine/Models/Item/ModelEvaluators/CompositeEvaluator.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

item_model::CompositeEvaluator::CompositeEvaluator(GameContext& context,
                                                   const json& data)
    : Evaluator{context} {
  Parse(data);
}
item_model::CompositeEvaluator::~CompositeEvaluator() = default;

void item_model::CompositeEvaluator::Evaluate(const EvaluatorContext& context,
                                              EvaluatedModels& models) const {
  for (const auto& evaluator : evaluators_) {
    evaluator->Evaluate(context, models);
  }
}

void item_model::CompositeEvaluator::Parse(const json& data) {
  // Checks if there is a list of models to parse
  if (!data.contains("models")) {
    LOG_WARN("Incomplete composite model!");
    error_state_ = true;
    return;
  }

  for (const auto& node : data["models"].items()) {
    auto evaluator = GetEvaluator(context_, node.value());
    if (evaluator == nullptr) {
      LOG_WARN("Skipping child composite model.");
    } else {
      evaluators_.push_back(std::move(evaluator));
    }
  }
}

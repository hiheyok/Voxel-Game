#include "RenderEngine/Models/Item/ModelEvaluators/ConditionEvaluator.h"

#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

item_model::ConditionEvaluator::ConditionEvaluator(GameContext& context,
                                                   const json& data)
    : Evaluator{context} {
  Parse(data);
}
item_model::ConditionEvaluator::~ConditionEvaluator() = default;

void item_model::ConditionEvaluator::Evaluate(const EvaluatorContext& context,
                                              EvaluatedModels& models) const {
  const auto& it = context.bool_conditions_.find(condition_);
  if (it != context.bool_conditions_.end() && it->second) {
    on_true_->Evaluate(context, models);
  } else {
    on_false_->Evaluate(context, models);
  }
}

void item_model::ConditionEvaluator::Parse(const json& data) {
  // Verify that it has all of the properties
  if (!data.contains("on_true") || !data.contains("on_false") ||
      !data.contains("property")) {
    error_state_ = true;
    LOG_WARN("Incomplete conditional model.");
    return;
  }

  const std::string& condition = data["property"].get<std::string>();

  const auto& it = kBooleanConditionMap.find(condition);

  if (it == kBooleanConditionMap.end()) {
    error_state_ = true;
    LOG_WARN("Invalid condition '{}' when parsing json!", condition);
    return;
  }

  auto on_true = GetEvaluator(context_, data["on_true"]); 
  auto on_false = GetEvaluator(context_, data["on_false"]); 

  if (on_true == nullptr || on_false == nullptr) {
    error_state_ = true;
    LOG_WARN("Failed to parse on_true or/and on_false");
    return;
  }

  on_true_ = std::move(on_true);
  on_false_ = std::move(on_false);
  condition_ = it->second;
}

// Checks if it has a condition

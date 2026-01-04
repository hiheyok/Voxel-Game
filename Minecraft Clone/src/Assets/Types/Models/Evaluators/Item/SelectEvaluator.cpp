#include "Assets/Types/Models/Evaluators/Item/SelectEvaluator.h"

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"
#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"
#include "Core/GameContext/GameContext.h"

using namespace item_model;

SelectEvaluator::SelectEvaluator(GameContext& context, const json& data)
    : Evaluator{context} {
  Parse(data);
}
SelectEvaluator::~SelectEvaluator() = default;

void SelectEvaluator::Evaluate(const EvaluatorContext& context,
                               EvaluatedModels& models) const {
  const Evaluator* evaluator = select_->GetEvaluator(context);
  if (evaluator != nullptr) {
    return evaluator->Evaluate(context, models);
  }

  if (fallback_ != nullptr) {
    return fallback_->Evaluate(context, models);
  }
}

void SelectEvaluator::Parse(const json& data) {
  select_ = PropertyValueProvider::GetProvider(context_, data);

  if (data.contains("fallback")) {
    fallback_ = Evaluator::GetEvaluator(context_, data["fallback"]);
  }
}

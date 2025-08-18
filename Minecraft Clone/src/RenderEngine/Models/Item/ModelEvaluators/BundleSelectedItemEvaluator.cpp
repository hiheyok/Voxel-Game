#include "RenderEngine/Models/Item/ModelEvaluators/BundleSelectedItemEvaluator.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

item_model::BundleSelectedItemEvaluator::BundleSelectedItemEvaluator(
    GameContext& context, const json& data)
    : Evaluator{context} {
  Parse(data);
}
item_model::BundleSelectedItemEvaluator::~BundleSelectedItemEvaluator() =
    default;

void item_model::BundleSelectedItemEvaluator::Evaluate(
    const EvaluatorContext& context, EvaluatedModels& models) const {}

void item_model::BundleSelectedItemEvaluator::Parse(const json& data) {}

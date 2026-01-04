#include "Assets/Types/Models/Evaluators/Item/BundleSelectedItemEvaluator.h"

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"
#include "Core/GameContext/GameContext.h"

using namespace item_model;

BundleSelectedItemEvaluator::BundleSelectedItemEvaluator(GameContext& context,
                                                         const json& data)
    : Evaluator{context} {
  Parse(data);
}

BundleSelectedItemEvaluator::~BundleSelectedItemEvaluator() = default;

void BundleSelectedItemEvaluator::Evaluate(const EvaluatorContext& context,
                                           EvaluatedModels& models) const {}

void BundleSelectedItemEvaluator::Parse(const json& data) {}

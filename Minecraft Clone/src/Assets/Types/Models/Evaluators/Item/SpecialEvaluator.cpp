#include "Assets/Types/Models/Evaluators/Item/SpecialEvaluator.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

using namespace item_model;

SpecialEvaluator::SpecialEvaluator(GameContext& context, const json& data)
    : Evaluator{context} {
  Parse(data);
}
SpecialEvaluator::~SpecialEvaluator() = default;

void SpecialEvaluator::Evaluate(const EvaluatorContext& context,
                                EvaluatedModels& models) const {}

void SpecialEvaluator::Parse(const json& data) {}

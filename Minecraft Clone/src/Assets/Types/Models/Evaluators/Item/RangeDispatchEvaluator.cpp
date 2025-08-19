#include "Assets/Types/Models/Evaluators/Item/RangeDispatchEvaluator.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

using namespace item_model;

RangeDispatchEvaluator::RangeDispatchEvaluator(GameContext& context,
                                               const json& data)
    : Evaluator{context} {
  Parse(data);
}

RangeDispatchEvaluator::~RangeDispatchEvaluator() = default;

void RangeDispatchEvaluator::Evaluate(const EvaluatorContext& context,
                                      EvaluatedModels& models) const {}

void RangeDispatchEvaluator::Parse(const json& data) {}

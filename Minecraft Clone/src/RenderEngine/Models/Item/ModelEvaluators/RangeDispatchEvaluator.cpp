#include "RenderEngine/Models/Item/ModelEvaluators/RangeDispatchEvaluator.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

item_model::RangeDispatchEvaluator::RangeDispatchEvaluator(GameContext& context,
                                                           const json& data)
    : Evaluator{context} {
  Parse(data);
}
item_model::RangeDispatchEvaluator::~RangeDispatchEvaluator() = default;

void item_model::RangeDispatchEvaluator::Evaluate(
    const EvaluatorContext& context, EvaluatedModels& models) const {}

void item_model::RangeDispatchEvaluator::Parse(const json& data) {}

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/Models/Item/ModelEvaluators/EmptyEvaluator.h"
#include "Utils/LogUtils.h"

item_model::EmptyEvaluator::EmptyEvaluator(GameContext& context,
                                           const json& data)
    : Evaluator{context} {
  Parse(data);
}
item_model::EmptyEvaluator::~EmptyEvaluator() = default;

void item_model::EmptyEvaluator::Evaluate(const EvaluatorContext& context,
                                          EvaluatedModels& models) const {}

void item_model::EmptyEvaluator::Parse(const json& data) {}

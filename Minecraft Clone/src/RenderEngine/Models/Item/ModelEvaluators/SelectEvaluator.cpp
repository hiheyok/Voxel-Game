#include "RenderEngine/Models/Item/ModelEvaluators/SelectEvaluator.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

item_model::SelectEvaluator::SelectEvaluator(GameContext& context,
                                             const json& data)
    : Evaluator{context} {
  Parse(data);
}
item_model::SelectEvaluator::~SelectEvaluator() = default;

void item_model::SelectEvaluator::Evaluate(const EvaluatorContext& context,
                                           EvaluatedModels& models) const {}

void item_model::SelectEvaluator::Parse(const json& data) {}

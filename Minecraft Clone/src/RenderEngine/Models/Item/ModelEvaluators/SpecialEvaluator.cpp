#include "RenderEngine/Models/Item/ModelEvaluators/SpecialEvaluator.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

item_model::SpecialEvaluator::SpecialEvaluator(GameContext& context,
                                               const json& data)
    : Evaluator{context} {
  Parse(data);
}
item_model::SpecialEvaluator::~SpecialEvaluator() = default;

void item_model::SpecialEvaluator::Evaluate(const EvaluatorContext& context,
                                            EvaluatedModels& models) const {}

void item_model::SpecialEvaluator::Parse(const json& data) {}

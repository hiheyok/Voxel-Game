#include "Assets/Types/Models/Evaluators/Item/EmptyEvaluator.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

using namespace item_model;

EmptyEvaluator::EmptyEvaluator(GameContext& context, const json& data)
    : Evaluator{context} {
  Parse(data);
}
EmptyEvaluator::~EmptyEvaluator() = default;

void EmptyEvaluator::Evaluate(const EvaluatorContext& context,
                              EvaluatedModels& models) const {}

void EmptyEvaluator::Parse(const json& data) {}

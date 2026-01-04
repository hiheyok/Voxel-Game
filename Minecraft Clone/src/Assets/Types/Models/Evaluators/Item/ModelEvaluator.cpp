#include "Assets/Types/Models/Evaluators/Item/ModelEvaluator.h"

#include <string>

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Registry/ResourceLocation.h"
#include "Utils/LogUtils.h"

using namespace item_model;
using std::string;

ModelEvaluator::ModelEvaluator(GameContext& context, const json&)
    : Evaluator{context} {}
ModelEvaluator::~ModelEvaluator() = default;

void ModelEvaluator::SetResourceLocation(ResourceLocation location) {
  location_ = location;
}

void ModelEvaluator::Evaluate(const EvaluatorContext& context,
                              EvaluatedModels& models) const {
  models.push_back(location_);
}

void ModelEvaluator::Parse(const json& data) {
  // Ignore tints for now
  if (!data.contains("model")) {
    LOG_WARN("Incomplete model!");
    error_state_ = true;
    return;
  }

  location_ = data["model"].get<string>();
}

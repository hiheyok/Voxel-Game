#include "Assets/Types/Models/Evaluators/Item/Property/ContextEntityTypeProvider.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

using namespace item_model;
using std::move;
using std::string;
using std::unique_ptr;

ContextEntityTypeProvider::ContextEntityTypeProvider(GameContext& context,
                                                     const json& data)
    : PropertyValueProvider{context} {
  Parse(data);
}
ContextEntityTypeProvider::~ContextEntityTypeProvider() = default;

const Evaluator* ContextEntityTypeProvider::GetEvaluator(
    const EvaluatorContext&) const {
  return nullptr;
}

void ContextEntityTypeProvider::Parse(const json& data) {
  if (!data.contains("cases")) {
    LOG_WARN("Incomplete context entity type provider.");
    status_ = Status::kError;
  }

  for (const auto& model_case : data["cases"]) {
    if (!model_case.contains("model") || !model_case.contains("when")) {
      LOG_WARN("Skipping case. Incomplete case.");
      continue;
    }

    const string& when = model_case["when"].get<string>();

    unique_ptr<Evaluator> evaluator =
        Evaluator::GetEvaluator(context_, model_case["model"]);
    if (evaluator == nullptr) {
      LOG_WARN("Skipping case. Invalid model.");
      continue;
    }

    evaluators_[when] = move(evaluator);
  }
}

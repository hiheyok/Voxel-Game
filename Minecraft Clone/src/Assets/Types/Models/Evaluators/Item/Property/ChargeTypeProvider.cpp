#include "Assets/Types/Models/Evaluators/Item/Property/ChargeTypeProvider.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

using namespace item_model;
using std::move;
using std::string;
using std::unique_ptr;

decltype(ChargeTypeProvider::kValidChoices) ChargeTypeProvider::kValidChoices =
    {"none", "rocket", "arrow"};

ChargeTypeProvider::ChargeTypeProvider(GameContext& context, const json& data)
    : PropertyValueProvider{context} {
  Parse(data);
}
ChargeTypeProvider::~ChargeTypeProvider() = default;

const Evaluator* ChargeTypeProvider::GetEvaluator(
    const EvaluatorContext&) const {
  return nullptr;
}

void ChargeTypeProvider::Parse(const json& data) {
  if (!data.contains("cases")) {
    LOG_WARN("Incomplete charge type provider.");
    status_ = Status::kError;
  }

  for (const auto& model_case : data["cases"]) {
    if (!model_case.contains("model") || !model_case.contains("when")) {
      LOG_WARN("Skipping case. Incomplete case.");
      continue;
    }

    const string& when = model_case["when"].get<string>();

    if (!kValidChoices.contains(when)) {
      LOG_WARN("Skipping case. Invalid type.");
      continue;
    }

    unique_ptr<Evaluator> evaluator =
        Evaluator::GetEvaluator(context_, model_case["model"]);
    if (evaluator == nullptr) {
      LOG_WARN("Skipping case. Invalid model.");
      continue;
    }

    evaluators_[when] = move(evaluator);
  }
}
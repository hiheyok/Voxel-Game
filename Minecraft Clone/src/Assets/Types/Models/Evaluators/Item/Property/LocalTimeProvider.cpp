#include "Assets/Types/Models/Evaluators/Item/Property/LocalTimeProvider.h"

#include <memory>
#include <string>

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"
#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"
#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

using namespace item_model;
using std::move;
using std::string;
using std::unique_ptr;

LocalTimeProvider::LocalTimeProvider(GameContext& context, const json& data)
    : PropertyValueProvider{context} {
  Parse(data);
}

LocalTimeProvider::~LocalTimeProvider() = default;

const Evaluator* LocalTimeProvider::GetEvaluator(
    const EvaluatorContext&) const {
  return nullptr;
}

void LocalTimeProvider::Parse(const json& data) {
  if (!data.contains("cases") || !data.contains("pattern")) {
    LOG_WARN("Incomplete local time provider.");
    status_ = Status::kError;
  }

  pattern_ = data["pattern"].get<string>();

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

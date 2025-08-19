#pragma once

#include <memory>
#include <vector>

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"
#include "Core/Registry/ResourceLocation.h"

namespace item_model {
class ConditionEvaluator : public Evaluator {
 public:
  ConditionEvaluator(GameContext&, const json&);
  ~ConditionEvaluator();

  void Evaluate(const EvaluatorContext&, EvaluatedModels&) const override;

 private:
  void Parse(const json&);

  std::unique_ptr<Evaluator> on_true_;
  std::unique_ptr<Evaluator> on_false_;
  BooleanCondition condition_;
};
}  // namespace item_model

#pragma once

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"

namespace item_model {
class SpecialEvaluator : public Evaluator {
 public:
  SpecialEvaluator(GameContext&, const json&);
  ~SpecialEvaluator();

  void Evaluate(const EvaluatorContext&, EvaluatedModels&) const override;

 private:
  void Parse(const json&);
};
}  // namespace item_model

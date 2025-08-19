#pragma once

#include <memory>
#include <vector>

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"

namespace item_model {
class BundleSelectedItemEvaluator : public Evaluator {
 public:
  BundleSelectedItemEvaluator(GameContext&, const json&);
  ~BundleSelectedItemEvaluator();

  void Evaluate(const EvaluatorContext&, EvaluatedModels&) const override;

 private:
  void Parse(const json&);
};
}  // namespace item_model

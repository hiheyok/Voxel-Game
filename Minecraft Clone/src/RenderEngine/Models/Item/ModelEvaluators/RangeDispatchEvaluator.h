#pragma once

#include "RenderEngine/Models/Item/ModelEvaluators/Evaluator.h"

namespace item_model {
class RangeDispatchEvaluator : public Evaluator {
 public:
  RangeDispatchEvaluator(GameContext&, const json&);
  ~RangeDispatchEvaluator();

  void Evaluate(const EvaluatorContext&, EvaluatedModels&) const override;

 private:
  void Parse(const json&);
};
}  // namespace item_model

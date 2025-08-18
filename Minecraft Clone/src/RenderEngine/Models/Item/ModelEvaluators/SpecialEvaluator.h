#pragma once

#include "RenderEngine/Models/Item/ModelEvaluators/Evaluator.h"

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

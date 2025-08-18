#pragma once

#include "RenderEngine/Models/Item/ModelEvaluators/Evaluator.h"

namespace item_model {
class SelectEvaluator : public Evaluator {
 public:
  SelectEvaluator(GameContext&, const json&);
  ~SelectEvaluator();

  void Evaluate(const EvaluatorContext&, EvaluatedModels&) const override;

 private:
  void Parse(const json&);
};
}  // namespace item_model

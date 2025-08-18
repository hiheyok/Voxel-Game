#pragma once

#include <memory>
#include <vector>

#include "RenderEngine/Models/Item/ModelEvaluators/Evaluator.h"

namespace item_model {
class EmptyEvaluator : public Evaluator {
 public:
  EmptyEvaluator(GameContext&, const json&);
  ~EmptyEvaluator();

  void Evaluate(const EvaluatorContext&, EvaluatedModels&) const override;

 private:
  void Parse(const json&);
};
}  // namespace item_model

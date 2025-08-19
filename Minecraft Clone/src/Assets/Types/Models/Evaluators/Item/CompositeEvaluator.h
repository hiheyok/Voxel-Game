#pragma once

#include <memory>
#include <vector>

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"
#include "Core/Registry/ResourceLocation.h"

namespace item_model {
class CompositeEvaluator : public Evaluator {
 public:
  CompositeEvaluator(GameContext&, const json&);
  ~CompositeEvaluator();

  void Evaluate(const EvaluatorContext&, EvaluatedModels&) const override;

 private:
  void Parse(const json&);

  EvaluatorList evaluators_;
};
}  // namespace item_model

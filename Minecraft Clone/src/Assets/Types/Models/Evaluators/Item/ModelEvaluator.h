#pragma once

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"

namespace item_model {
class ModelEvaluator : public Evaluator {
 public:
  ModelEvaluator(GameContext&, const json&);
  ~ModelEvaluator();

  void SetResourceLocation(ResourceLocation location);

  void Evaluate(const EvaluatorContext&, EvaluatedModels&) const override;

 private:
  void Parse(const json&);

  ResourceLocation location_;
};
}  // namespace item_model

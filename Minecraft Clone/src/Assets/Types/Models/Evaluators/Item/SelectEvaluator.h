#pragma once

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"

namespace item_model {

class PropertyValueProvider;

class SelectEvaluator : public Evaluator {
 public:
  SelectEvaluator(GameContext&, const json&);
  ~SelectEvaluator();

  void Evaluate(const EvaluatorContext&, EvaluatedModels&) const override;

 private:
  void Parse(const json&);

  std::unique_ptr<PropertyValueProvider> select_;
  std::unique_ptr<Evaluator> fallback_;
};
}  // namespace item_model

#pragma once

#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"

namespace item_model {
class ComponentProvider : public PropertyValueProvider {
 public:
  ComponentProvider(GameContext&, const json&);
  ~ComponentProvider();

  const Evaluator* GetEvaluator(const EvaluatorContext&) const override;

 private:
  void Parse(const json&);
};

}  // namespace item_model

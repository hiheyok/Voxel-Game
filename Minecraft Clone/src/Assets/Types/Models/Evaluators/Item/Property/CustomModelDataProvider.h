#pragma once

#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"

namespace item_model {
class CustomModelDataProvider : public PropertyValueProvider {
 public:
  CustomModelDataProvider(GameContext&, const json&);
  ~CustomModelDataProvider();

  const Evaluator* GetEvaluator(const EvaluatorContext&) const override;

 private:
  void Parse(const json&);
};

}  // namespace item_model

#pragma once

#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"

namespace item_model {
class TrimMaterialProvider : public PropertyValueProvider {
 public:
  TrimMaterialProvider(GameContext&, const json&);
  ~TrimMaterialProvider();

  const Evaluator* GetEvaluator(const EvaluatorContext&) const override;

 private:
  void Parse(const json&);

  FastHashMap<std::string, std::unique_ptr<Evaluator>> evaluators_;
};

}  // namespace item_model

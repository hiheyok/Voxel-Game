#pragma once

#include "Core/Typenames.h"
#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"

namespace item_model {
class ContextDimensionProvider : public PropertyValueProvider {
 public:
  ContextDimensionProvider(GameContext&, const json&);
  ~ContextDimensionProvider();

  const Evaluator* GetEvaluator(const EvaluatorContext&) const override;

 private:
  void Parse(const json&);

  FastHashMap<std::string, std::unique_ptr<Evaluator>> evaluators_;
};

}  // namespace item_model

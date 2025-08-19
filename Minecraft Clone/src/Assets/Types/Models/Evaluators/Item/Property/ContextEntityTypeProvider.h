#pragma once

#include "Core/Typenames.h"
#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"

namespace item_model {
class ContextEntityTypeProvider : public PropertyValueProvider {
 public:
  ContextEntityTypeProvider(GameContext&, const json&);
  ~ContextEntityTypeProvider();

  const Evaluator* GetEvaluator(const EvaluatorContext&) const override;

 private:
  void Parse(const json&);

  FastHashMap<std::string, std::unique_ptr<Evaluator>> evaluators_;
};

}  // namespace item_model

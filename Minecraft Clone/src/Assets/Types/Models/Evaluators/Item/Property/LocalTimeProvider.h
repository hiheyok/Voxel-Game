#pragma once

#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"
#include "Core/Typenames.h"

namespace item_model {
class LocalTimeProvider : public PropertyValueProvider {
 public:
  LocalTimeProvider(GameContext&, const json&);
  ~LocalTimeProvider();

  const Evaluator* GetEvaluator(const EvaluatorContext&) const override;

 private:
  void Parse(const json&);

  std::string pattern_;

  FastHashMap<std::string, std::unique_ptr<Evaluator>> evaluators_;
};

}  // namespace item_model

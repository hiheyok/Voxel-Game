#pragma once

#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"
#include "Core/Typenames.h"

namespace item_model {
class DisplayContextProvider : public PropertyValueProvider {
 public:
  DisplayContextProvider(GameContext&, const json&);
  ~DisplayContextProvider();

  const Evaluator* GetEvaluator(const EvaluatorContext&) const override;

 private:
  void Parse(const json&);

  static const FastHashSet<std::string_view> kValidChoices;

  FastHashMap<std::string, std::unique_ptr<Evaluator>> evaluators_;
};

}  // namespace item_model

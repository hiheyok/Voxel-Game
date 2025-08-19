#pragma once
#include <string_view>

#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"

namespace item_model {
class MainHandProvider : public PropertyValueProvider {
 public:
  MainHandProvider(GameContext&, const json&);
  ~MainHandProvider();

  const Evaluator* GetEvaluator(const EvaluatorContext&) const override;

  // private:
  void Parse(const json&);

  static const FastHashSet<std::string_view> kValidChoices;

  std::unique_ptr<Evaluator> left_;
  std::unique_ptr<Evaluator> right_;
};

}  // namespace item_model

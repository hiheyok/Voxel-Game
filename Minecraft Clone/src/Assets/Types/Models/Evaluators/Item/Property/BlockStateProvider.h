#pragma once

#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"
#include "Core/Typenames.h"

namespace item_model {
class BlockStateProvider : public PropertyValueProvider {
 public:
  BlockStateProvider(GameContext&, const json&);
  ~BlockStateProvider();

  const Evaluator* GetEvaluator(const EvaluatorContext&) const override;

 private:
  void Parse(const json&);

  std::string block_state_property_;
  FastHashMap<std::string, std::unique_ptr<Evaluator>> evaluators_;
};

}  // namespace item_model

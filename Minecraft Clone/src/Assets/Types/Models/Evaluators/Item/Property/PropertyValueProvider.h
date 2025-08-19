#pragma once

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"

namespace item_model {

class PropertyValueProvider {
 public:
  PropertyValueProvider(GameContext&);
  virtual ~PropertyValueProvider();

  virtual const Evaluator* GetEvaluator(const EvaluatorContext&) const = 0;

  static std::unique_ptr<PropertyValueProvider> GetProvider(GameContext&,
                                                            const json&);

 protected:
  GameContext& context_;
  Status status_;
};

}  // namespace item_model
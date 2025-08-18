#include "RenderEngine/Models/Item/ModelEvaluators/Evaluator.h"

namespace item_model {

class PropertyValueProvider {
 public:
  PropertyValueProvider(GameContext&);
  virtual ~PropertyValueProvider();
  
  virtual const Evaluator* GetEvaluator(EvaluatorContext&) const noexcept = 0;

 protected:
  GameContext& context;
};

}  // namespace item_model
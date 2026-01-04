#include "Assets/Types/Models/Evaluators/Item/Property/CustomModelDataProvider.h"

#include <memory>
#include <string>

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"
#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"
#include "Core/GameContext/GameContext.h"

using namespace item_model;
using std::move;
using std::string;
using std::unique_ptr;

CustomModelDataProvider::CustomModelDataProvider(GameContext& context,
                                                 const json& data)
    : PropertyValueProvider{context} {
  Parse(data);
}
CustomModelDataProvider::~CustomModelDataProvider() = default;

const Evaluator* CustomModelDataProvider::GetEvaluator(
    const EvaluatorContext&) const {
  return nullptr;
}

void CustomModelDataProvider::Parse(const json& data) {}

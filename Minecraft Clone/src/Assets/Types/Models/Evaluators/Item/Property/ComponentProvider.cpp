#include "Assets/Types/Models/Evaluators/Item/Property/ComponentProvider.h"

#include <memory>
#include <string>

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"
#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"
#include "Core/GameContext/GameContext.h"

using namespace item_model;
using std::move;
using std::string;
using std::unique_ptr;

ComponentProvider::ComponentProvider(GameContext& context, const json& data)
    : PropertyValueProvider{context} {
  Parse(data);
}
ComponentProvider::~ComponentProvider() = default;

const Evaluator* ComponentProvider::GetEvaluator(
    const EvaluatorContext&) const {
  return nullptr;
}

void ComponentProvider::Parse(const json& data) {}

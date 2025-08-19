#include "Assets/Types/Models/Evaluators/Item/Property/ComponentProvider.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

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

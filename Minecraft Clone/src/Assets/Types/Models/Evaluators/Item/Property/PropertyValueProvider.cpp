#include "Assets/Types/Models/Evaluators/Item/Property/PropertyValueProvider.h"

#include <memory>
#include <string>

#include "Assets/Types/Models/Evaluators/Item/Evaluator.h"
#include "Assets/Types/Models/Evaluators/Item/Property/BlockStateProvider.h"
#include "Assets/Types/Models/Evaluators/Item/Property/ChargeTypeProvider.h"
#include "Assets/Types/Models/Evaluators/Item/Property/ComponentProvider.h"
#include "Assets/Types/Models/Evaluators/Item/Property/ContextDimensionProvider.h"
#include "Assets/Types/Models/Evaluators/Item/Property/ContextEntityTypeProvider.h"
#include "Assets/Types/Models/Evaluators/Item/Property/CustomModelDataProvider.h"
#include "Assets/Types/Models/Evaluators/Item/Property/DisplayContextProvider.h"
#include "Assets/Types/Models/Evaluators/Item/Property/LocalTimeProvider.h"
#include "Assets/Types/Models/Evaluators/Item/Property/MainHandProvider.h"
#include "Assets/Types/Models/Evaluators/Item/Property/TrimMaterialProvider.h"
#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

using namespace item_model;
using std::make_unique;
using std::string;
using std::unique_ptr;

PropertyValueProvider::PropertyValueProvider(GameContext& context)
    : context_{context}, status_{Status::kUnloaded} {}
PropertyValueProvider::~PropertyValueProvider() = default;

unique_ptr<PropertyValueProvider> PropertyValueProvider::GetProvider(
    GameContext& context, const json& data) {
  if (!data.contains("property")) {
    LOG_STATIC_WARN(context.logger_, "Incomplete selection type!");
    return nullptr;
  }

  const string& property = data["property"].get<string>();

  if (property == "minecraft:block_state") {
    return make_unique<BlockStateProvider>(context, data);
  } else if (property == "minecraft:charge_type") {
    return make_unique<ChargeTypeProvider>(context, data);
  } else if (property == "minecraft:component") {
    return make_unique<ComponentProvider>(context, data);
  } else if (property == "minecraft:context_dimension") {
    return make_unique<ContextDimensionProvider>(context, data);
  } else if (property == "minecraft:context_entity_type") {
    return make_unique<ContextEntityTypeProvider>(context, data);
  } else if (property == "minecraft:display_context") {
    return make_unique<DisplayContextProvider>(context, data);
  } else if (property == "minecraft:local_time") {
    return make_unique<LocalTimeProvider>(context, data);
  } else if (property == "minecraft:main_hand") {
    return make_unique<MainHandProvider>(context, data);
  } else if (property == "minecraft:trim_material ") {
    return make_unique<TrimMaterialProvider>(context, data);
  } else if (property == "minecraft:custom_model_data") {
    return make_unique<CustomModelDataProvider>(context, data);
  } else {
    LOG_STATIC_WARN(context.logger_, "Invalid property!");
  }

  return nullptr;
}

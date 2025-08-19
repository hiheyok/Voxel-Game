#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

#include "Core/Typenames.h"

class GameContext;
class ResourceLocation;

namespace item_model {

class Evaluator;

using EvaluatedModels = std::vector<ResourceLocation>;
using EvaluatorList = std::vector<std::unique_ptr<Evaluator>>;
using json = nlohmann::json;

enum class BooleanCondition {
  kBroken,
  kBundleSelect,
  kCarried,
  kComponent,
  kDamaged,
  kExtendedView,
  kFishingRodCast,
  kHasComponent,
  kKeybindDown,
  kSelected,
  kUsingItem,
  kViewEntity,
  kCustomModelData
};

enum class Status { kError, kSuccess, kUnloaded };

static const inline FastHashMap<std::string, BooleanCondition>
    kBooleanConditionMap = {
        {"minecraft:broken", BooleanCondition::kBroken},
        {"minecraft:bundle/has_selected_item", BooleanCondition::kBundleSelect},
        {"minecraft:carried", BooleanCondition::kCarried},
        {"minecraft:component", BooleanCondition::kComponent},
        {"minecraft:damaged", BooleanCondition::kDamaged},
        {"minecraft:extended_view", BooleanCondition::kExtendedView},
        {"minecraft:fishing_rod/cast", BooleanCondition::kFishingRodCast},
        {"minecraft:has_component", BooleanCondition::kHasComponent},
        {"minecraft:keybind_down", BooleanCondition::kKeybindDown},
        {"minecraft:selected", BooleanCondition::kSelected},
        {"minecraft:using_item", BooleanCondition::kUsingItem},
        {"minecraft:view_entity ", BooleanCondition::kViewEntity},
        {"minecraft:custom_model_data", BooleanCondition::kCustomModelData}};

struct EvaluatorContext {
  FastHashMap<BooleanCondition, bool> bool_conditions_;
};

class Evaluator {
 public:
  Evaluator(GameContext&);
  virtual ~Evaluator();

  Evaluator& operator=(const Evaluator&) = delete;
  Evaluator(const Evaluator&) = delete;

  Evaluator& operator=(Evaluator&&) = delete;
  Evaluator(Evaluator&&) = delete;

  virtual void Evaluate(const EvaluatorContext&, EvaluatedModels&) const = 0;
  // From the data it finds the type and create the evaluator specific to that
  // type
  static std::unique_ptr<Evaluator> GetEvaluator(GameContext& context,
                                                 const json& model_data);

 protected:
  GameContext& context_;
  // Used to detect if there is an error while parsing
  bool error_state_;
};
}  // namespace item_model

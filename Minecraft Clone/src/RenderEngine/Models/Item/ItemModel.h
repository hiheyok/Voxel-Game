#pragma once

/*
Composite
- type: minecraft:composite
- models: list of models to render

Condition
- type: minecraft:condition
- property:minecraft:broken, minecraft:bundle/has_selected_item,
  minecraft:carried, minecraft:component, minecraft:damaged,
  minecraft:extended_view, minecraft:fishing_rod/cast, minecraft:has_component,
  minecraft:keybind_down, minecraft:selected, minecraft:using_item,
  minecraft:view_entity or minecraft:custom_model_data
  ....

ItemModelContext
 - This provides all of the infomation on evaluating all of the conditions

enum class ItemModelType {...};

ItemModelEvaluatorInterface
 - Abstract class for all of the model types
 - ItemModelType type

ItemModelEvaluatorModel
 - std::optional<ItemModelTint>
-
*/

class ItemModel {
 public:
 private:
};

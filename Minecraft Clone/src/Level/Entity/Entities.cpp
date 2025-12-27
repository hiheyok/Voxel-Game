// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Entity/Entities.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "Assets/AssetManager.h"
#include "Assets/Types/EntityModel.h"
#include "Core/GameContext/GameContext.h"
#include "Level/Entity/Type/Types.h"
#include "Utils/Assert.h"
#include "Utils/LogUtils.h"

using json = nlohmann::json;

EntitiesList::EntitiesList(GameContext& context)
    : context_{context},
      entity_type_list_(static_cast<int>(EntityType::kMaxVal), nullptr) {}

EntityType EntitiesList::RegisterEntity(std::string entity_name,
                                        EntityTypeEnums type,
                                        EntityType entity_type) {
  EntityTypeData* new_entity = nullptr;

  switch (type) {
    case ENTITY_PASSIVE:
      new_entity = static_cast<EntityTypeData*>(new Passive(context_));
      break;
    case ENTITY_FALLING_BLOCK:
      new_entity = static_cast<EntityTypeData*>(new FallingBlock(context_));
      break;
    case ENTITY_HOSTILE:
      new_entity = static_cast<EntityTypeData*>(new Hostile(context_));
      break;
  }

  GAME_ASSERT(new_entity != nullptr, "Failed to register entity");

  new_entity->entity_name_ = entity_name;

  LOG_INFO("Registered new entity: {} | EntityID: {}", entity_name,
           static_cast<int>(entity_type));

  new_entity->id_ = entity_type;

  entity_type_list_[static_cast<int>(entity_type)] = new_entity;
  entity_name_id_[entity_name] = entity_type;

  return entity_type;
}

void EntitiesList::InitializeModels() {
  // trash code
  for (auto entity : entity_type_list_) {
    if (entity == nullptr) continue;
    const std::string& name = entity->entity_name_;
    AssetHandle<EntityModel> model = context_.assets_->GetEntityModel(name);
    if (!model.HasValue()) continue;
    entity->ChangeHitboxSize(model->hitbox_);
  }
}

void EntitiesList::Initialize() { InitializeModels(); }

EntityTypeData* EntitiesList::GetEntity(EntityType id) {
  return entity_type_list_[static_cast<int>(id)];
}

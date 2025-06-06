// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Core/GameContext/GameContext.h"

#include <memory>

#include "Core/Options/Option.h"
#include "Level/Block/Blocks.h"
#include "Level/Entity/Entities.h"
#include "Level/Event/EventHandler.h"
#include "Level/Item/ItemTextureAtlas.h"
#include "Level/Item/Items.h"
#include "Level/TerrainGeneration/Generators/GeneratorType.h"
#include "RenderEngine/BlockModel/BlockModelManager.h"
#include "Utils/LogUtils.h"

GameContext::GameContext()
    : logger_{std::make_unique<LogUtils>()},
      options_{std::make_unique<Options>(*this)},
      blocks_{std::make_unique<BlockList>(*this)},
      entities_list_{std::make_unique<EntitiesList>(*this)},
      items_{std::make_unique<ItemList>(*this)},
      generators_{std::make_unique<GeneratorType>(*this)},
      event_handler_{std::make_unique<EventHandler>(*this)},
      block_model_manager_{std::make_unique<BlockModelManager>(*this)} {}
GameContext::~GameContext() = default;

void GameContext::InitializeGameContext() {
  items_->RegisterAll();
}
void GameContext::InitializeRenderingContext() {
  // Initialize objects
  item_atlas_ = std::make_unique<ItemTextureAtlas>(*this);


  block_model_manager_->LoadModels();
  item_atlas_->Initialize(512 * 16 * 2, 16 * 2 * 8);
  for (auto& item : items_->item_container_) {
    item_atlas_->AddItem(item.second);
  }
  entities_list_->Initialize();
}

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Core/GameContext/GameContext.h"

#include <filesystem>
#include <memory>

#include "Assets/AssetManager.h"
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
      assets_{std::make_unique<AssetManager>(*this)},
      block_model_manager_{std::make_unique<BlockModelManager>(*this)} {}
GameContext::~GameContext() = default;

void GameContext::InitializeGameContext() {
  items_->RegisterAll();
  assets_->Initialize();
  assets_->Load();

  // try {
  //   for (const auto& entry : std::filesystem::directory_iterator("assets")) {
  //     if (!entry.is_directory()) continue;
  //     std::string name = entry.path().filename().string();
  //     if (!strcmp(name.c_str(), "shaders")) continue;
  //     blocks_->AddAssets(name);
  //   }

  // } catch (std::filesystem::filesystem_error& e) {
  //   logger_->LogWarn("File System", e.what());
  // }
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

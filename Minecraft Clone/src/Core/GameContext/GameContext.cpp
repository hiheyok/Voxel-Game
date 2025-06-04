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
#include "Utils/LogUtils.h"

GameContext::GameContext()
    : logger_{std::make_unique<LogUtils>()},
      options_{std::make_unique<Options>(*this)},
      blocks_{std::make_unique<BlockList>(*this)},
      entities_list_{std::make_unique<EntitiesList>(*this)},
      items_{std::make_unique<ItemList>(*this)},
      item_atlas_{std::make_unique<ItemTextureAtlas>(*this)},
      generators_{std::make_unique<GeneratorType>(*this)},
      event_handler_{std::make_unique<EventHandler>(*this)} {}
GameContext::~GameContext() = default;

void GameContext::InitializeGameContext() {}
void GameContext::InitializeRenderingContext() {}

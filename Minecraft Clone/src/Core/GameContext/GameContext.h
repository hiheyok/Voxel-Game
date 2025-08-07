// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>

// This will include all of the game context
// Manages initialization

class BlockList;
class EntitiesList;
class LogUtils;
class Options;
class ItemList;
class GeneratorType;
class EventHandler;
class AssetManager;

// Rendering Context;
class ItemTextureAtlas;
class BlockModelManager;

class GameContext {
 public:
  GameContext();
  ~GameContext();

  GameContext(const GameContext&) = delete;
  GameContext(GameContext&&) = delete;

  void InitializeGameContext();
  void InitializeRenderingContext();

  std::unique_ptr<LogUtils> logger_;
  std::unique_ptr<Options> options_;
  std::unique_ptr<BlockList> blocks_;
  std::unique_ptr<EntitiesList> entities_list_;
  std::unique_ptr<ItemList> items_;
  std::unique_ptr<GeneratorType> generators_;
  std::unique_ptr<EventHandler> event_handler_;
  std::unique_ptr<AssetManager> assets_;

  // Rendering Context
  std::unique_ptr<ItemTextureAtlas> item_atlas_;
  std::unique_ptr<BlockModelManager> block_model_manager_;
};

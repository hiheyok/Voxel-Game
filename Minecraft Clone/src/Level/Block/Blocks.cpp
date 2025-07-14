// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Blocks.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "Core/GameContext/GameContext.h"
#include "FileManager/Files.h"
#include "Level/Block/Type/BlockTypes.h"
#include "RenderEngine/BlockModel/BlockModels.h"
#include "RenderEngine/BlockModel/ModelLoader.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"
#include "Utils/LogUtils.h"

using json = nlohmann::json;

BlockList::BlockList(GameContext& game_context)
    : game_context_{game_context} {}

BlockList::~BlockList() {
  for (const auto& obj : block_type_data_) {
    delete obj;
  }
}

BlockID BlockList::RegisterBlock(std::string blockName, Block* block) {
  BlockID id = static_cast<BlockID>(block_type_data_.size());

  block->id_ = id;
  block->block_name_ = blockName;

  block_type_data_.emplace_back(block);
  block_properties_.emplace_back(*block->properties_);

  block_id_name_data_[blockName] = id;

  game_context_.logger_->LogInfo(
      "BlockList::RegisterBlock",
      "Registered new block (ID): " + std::to_string(id));
  return id;
}

void BlockList::AddAssets(std::string namespaceIn) {
  try {
    std::vector<std::string> allOtherBlocks{};
    std::string path = "assets/" + namespaceIn + "/models/block";

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(path)) {
      if (entry.is_directory()) continue;
      std::string path = entry.path().string();
      size_t idx = path.find("/models/block") +
                   std::string("/models/block").length() + 1;
      std::string name =
          namespaceIn + ":" + path.substr(idx, path.size() - idx - 5);
      if (entry.path().extension() != ".json") continue;
      if (block_id_name_data_.count(name)) {
        continue;
      }

      allOtherBlocks.push_back(name);
    }

    for (std::string& name : allOtherBlocks) {
      RegisterBlock(name, new DefaultBlock(game_context_));
    }

  } catch (std::filesystem::filesystem_error& e) {
    game_context_.logger_->LogWarn("BlockList::AddAssets", e.what());
  }
}

Block* BlockList::GetBlockType(BlockID id) { return block_type_data_[id]; }

const BlockProperties& BlockList::GetBlockProperties(BlockID id) const {
  return block_properties_[id];
}

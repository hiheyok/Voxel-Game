// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Blocks.h"

#include <cstddef>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Typenames.h"
#include "Level/Block/Block.h"
#include "Level/Block/Type/DefaultBlock.h"
#include "Utils/LogUtils.h"

using json = nlohmann::json;
using std::string;
using std::vector;
using std::filesystem::filesystem_error;
using std::filesystem::recursive_directory_iterator;

BlockList::BlockList(GameContext& context) : context_{context} {}

BlockList::~BlockList() {
  for (const auto& obj : block_type_data_) {
    delete obj;
  }
}

BlockID BlockList::RegisterBlock(const string& block_name, Block* block) {
  BlockID id = static_cast<BlockID>(block_type_data_.size());

  block->id_ = id;
  block->block_name_ = block_name;

  block_type_data_.emplace_back(block);
  block_properties_.emplace_back(*block->properties_);

  block_id_name_data_[block_name] = id;

  LOG_INFO("Registered new block (ID) {}: {}", id, block_name);
  return id;
}

void BlockList::AddAssets(const string& namespace_in) {
  try {
    vector<string> allOtherBlocks{};
    string path = "assets/" + namespace_in + "/models/block";

    for (const auto& entry : recursive_directory_iterator(path)) {
      if (entry.is_directory()) continue;
      string path = entry.path().string();
      size_t idx =
          path.find("/models/block") + string("/models/block").length() + 1;
      string name =
          namespace_in + ":" + path.substr(idx, path.size() - idx - 5);
      if (entry.path().extension() != ".json") continue;
      if (block_id_name_data_.count(name)) {
        continue;
      }

      allOtherBlocks.push_back(name);
    }

    for (string& name : allOtherBlocks) {
      RegisterBlock(name, new DefaultBlock(context_));
    }

  } catch (filesystem_error& e) {
    LOG_WARN("{}", e.what());
  }
}

Block* BlockList::GetBlockType(BlockID id) { return block_type_data_[id]; }

const vector<Block*>& BlockList::GetBlockTypeList() const noexcept {
  return block_type_data_;
}

const BlockProperties& BlockList::GetBlockProperties(BlockID id) const {
  return block_properties_[id];
}

const vector<BlockProperties>& BlockList::GetBlockPropertyList() const {
  return block_properties_;
}

#include "Assets/Types/Models/Managers/BlockModelManager.h"

#include <vector>

#include "Assets/AssetHandle.h"
#include "Assets/Types/Models/Managers/ModelManager.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Registry/ResourceLocation.h"
#include "Level/Block/Block.h"
#include "Level/Block/Blocks.h"

using std::vector;

BlockModelManager::BlockModelManager(GameContext& context)
    : ModelManager{context} {}
BlockModelManager::~BlockModelManager() = default;

const BlockModelManager::ModelList& BlockModelManager::GetModels()
    const noexcept {
  return models_;
}

void BlockModelManager::Load() {
  const vector<Block*>& blocks = context_.blocks_->block_type_data_;
  models_.reserve(blocks.size());

  for (int i = 0; i < blocks.size(); ++i) {
    ResourceLocation location{blocks[i]->block_name_, "models/block/"};
    AssetHandle<RenderableModel> handle = LoadModel(location);
    models_.push_back(handle);
  }
}

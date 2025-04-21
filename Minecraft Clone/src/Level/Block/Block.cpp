#include "Level/Block/Block.h"
#include "RenderEngine/BlockModel/BlockModels.h"
#include "RenderEngine/BlockModel/ModelLoader.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"
#include "FileManager/Files.h"

Block::~Block() = default;
Block::Block() {
    properties_ = std::make_unique<BlockProperties>();
}

void Block::InitializeBlockModel(ModelLoader& modelLoader) {
    auto tokens = Tokenize(block_name_, ':');
    std::unique_ptr<BlockModel> model = modelLoader.GetModel(ResourceLocation("models/block/" + tokens.back() + ".json", tokens.front()));
    if (model == nullptr) {
        model = std::make_unique<BlockModel>();
    }
    model->is_initialized_ = true;
    model->FlattenVariables();
    model->BakeTextureRotation();
    block_model_data_ = std::move(model);
}

void Block::InitializeTexture(BlockTextureAtlas& textureAtlas) {
    for (auto& element : block_model_data_->elements_) {
        for (const auto& side : Directions()) {
            const std::string& path = element.faces_[side].reference_texture_;
            if (path.length() == 0) continue;

            auto tokens = Tokenize(path, ':');

            //Load texture
            ResourceLocation location{ "/textures/" + tokens.front() + ".png" , tokens.back() };

            int textureId = textureAtlas.AddBlockTexture(location);

            if (textureId == -1) {
                g_logger.LogWarn("BlockList::InitializeBlockModels", "Unable to load texture.");
                continue;
            }

            element.faces_[side].texture_id_ = textureId;
            element.faces_[side].texture_count_ = textureAtlas.GetBlockTextureCount(location);
            element.faces_[side].partially_transparent_pixel_ = textureAtlas.IsTexturePartiallyTransparent(location);
            element.faces_[side].fully_transparent_pixel_ = textureAtlas.IsTextureFullyTransparent(location);
        }
    }
}

BlockProperties::BlockProperties() = default;
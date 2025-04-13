#include <filesystem>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

#include "Level/Block/Blocks.h"
#include "Level/Block/Type/BlockTypes.h"
#include "FileManager/Files.h"
#include "RenderEngine/BlockModel/ModelLoader.h"
#include "RenderEngine/BlockModel/BlockModels.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"

// TODO : Implement model caching
using json = nlohmann::json;

BlockID BlockList::RegisterBlock(std::string blockName, Material* material, bool transparency, bool solid, bool isFluid) {
    BlockID id = static_cast<BlockID>(block_type_data_.size());

    Block* block = material->BuildNewBlockType();

    // MaterialType Type = material->type;

    block->id_ = id;
    block->properties_ = std::make_unique<BlockType>(transparency, solid, isFluid);
    block->texture_ = std::make_unique<BlockTexture>();
    block->block_name_ = blockName;

    block_type_data_.emplace_back(block);

    block_id_name_data_[blockName] = id;

    g_logger.LogInfo("BlockList::RegisterBlock", "Registered new block (ID): " + std::to_string(id));
    delete material; // replace w/ unique ptr later
    return id;
}

void BlockList::AddAssets(std::string namespaceIn) {
    try {
        std::vector<std::string> allOtherBlocks{};
        std::string path = "assets/"+ namespaceIn + "/models/block";

        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_directory()) continue;
            std::string path = entry.path().string();
            size_t idx = path.find("/models/block") + std::string("/models/block").length() + 1;
            std::string name = namespaceIn + ":" + path.substr(idx, path.size() - idx - 5);
            if (entry.path().extension() != ".json") continue;
            if (block_id_name_data_.count(name)) {
                continue;
            }

            allOtherBlocks.push_back(name);
        }

        for (std::string& name : allOtherBlocks) {
            RegisterBlock(name, new MaterialNone(), false, true, false);
        }

    }
    catch (std::filesystem::filesystem_error& e) {
        g_logger.LogWarn("BlockList::AddAssets", e.what());
    }
}


void BlockList::InitializeBlockModels()  {
    //add every single block  in the assets bc why not
    //try {
    //    for (const auto& entry : std::filesystem::directory_iterator("assets")) {
    //        if (!entry.is_directory()) continue;
    //        std::string name = entry.path().filename().string();
    //        if (!strcmp(name.c_str(), "shaders")) continue;
    //        AddAssets(name);
    //    }

    //}
    //catch (std::filesystem::filesystem_error& e) {
    //    g_logger.LogError("File System", e.what());
    //}
 
    //It will first go through the block models and create the models without loading the texture
    for (const auto& [name, ID] : block_id_name_data_) {
        auto tokens = Tokenize(name, ':');
        std::unique_ptr<BlockModel> model = model_loader_.GetModel(ResourceLocation("models/block/" + tokens.back() + ".json", tokens.front()));
        model->is_initialized_ = true;
        model->FlattenVariables();
        model->BakeTextureRotation();
        GetBlockType(ID)->block_model_data_ = std::move(model);
    }

    //Generate all the textures now
    for (auto& block : block_type_data_) {
        if (block->block_model_data_ == 0) continue;

        for (auto& element : block->block_model_data_->elements_) {
            for (int i = 0; i < 6; i++) {
                const std::string& path = element.faces_[i].reference_texture_;
                if (path.length() == 0) continue;

                auto Tokens = Tokenize(path, ':');

                //Load texture
                ResourceLocation location{ "/textures/" + Tokens.front() + ".png" , Tokens.back() };

                int textureId = block_texture_atlas_->AddBlockTexture(location);

                if (textureId == -1) {
                    g_logger.LogWarn("BlockList::InitializeBlockModels", "Unable to load texture.");
                    continue;
                }

                element.faces_[i].texture_id_ = textureId;
                element.faces_[i].texture_count_ = block_texture_atlas_->GetBlockTextureCount(location);
                element.faces_[i].partially_transparent_pixel_ = block_texture_atlas_->IsTexturePartiallyTransparent(location);
                element.faces_[i].fully_transparent_pixel_ = block_texture_atlas_->IsTextureFullyTransparent(location);
            }
        }
    }

    for (size_t i = 0; i < block_type_data_.size(); i++) {
        BlockModel model;

        if (block_type_data_[i]->block_model_data_ != nullptr) {
            model = *block_type_data_[i]->block_model_data_;
        }
        model.texture_variable_.clear();
        block_model_data_.emplace_back(model);
    }
}

void BlockList::Initialize() {
    int individualTexSize = 16;

    block_texture_atlas_ = std::make_unique<BlockTextureAtlas>(512, 512, individualTexSize, individualTexSize);

    InitializeBlockModels();

    block_texture_atlas_->LoadToGPU();
}

BlockList::BlockList() = default;

BlockList::~BlockList() {
    for (const auto& obj : block_type_data_) {
        delete obj;
    }

    block_type_data_.clear();
}

Block* BlockList::GetBlockType(BlockID id) {
    return block_type_data_[id];
}

const BlockModel& BlockList::GetBlockModelDereferenced(BlockID id) {
    return block_model_data_[id];
}
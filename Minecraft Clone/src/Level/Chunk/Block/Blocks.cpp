#include "Blocks.h"
#include "Type/BlockTypes.h"
#include <nlohmann/json.hpp>
#include "../../../FileManager/Files.h"
#include <fstream>
#include "../../../RenderEngine/BlockModel/ModelLoader.h"
#include "../../../Utils/Containers/skaHashmap.h"
#include <iostream>
#include <sys/types.h>
#include <filesystem>
#include <string>
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

    g_logger.LogInfo("Register", "Registered new block (ID): " + std::to_string(id));
    delete material; // replace w/ unique ptr later
    return id;
}

void BlockList::AddAssets(std::string namespaceIn) {
    try {
        std::vector<std::string> allOtherBlocks{};
        std::string path = "assets/"+ namespaceIn + "/models/block";

        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_directory()) continue;
            std::string name = namespaceIn + ":" + entry.path().filename().string().substr(0, entry.path().filename().string().length() - 5);//epic one liner
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
        g_logger.LogError("File System", e.what());
    }
}


void BlockList::InitializeBlockModels()  {
    //add every single block  in the assets bc why not
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator("assets")) {
            if (!entry.is_directory()) continue;
            std::string name = entry.path().filename().string();//epic one liner
            if (!strcmp(name.c_str(), "shaders")) continue;
            AddAssets(name);
        }

    }
    catch (std::filesystem::filesystem_error& e) {
        g_logger.LogError("File System", e.what());
    }

    FastHashMap<std::string, size_t> textureIds;
    FastHashMap<size_t, size_t> textureRepeatCount;
    FastHashMap<size_t, bool> textureTransparency;
    FastHashMap<size_t, bool> textureSeeThrough;
 
    //It will first go through the block models and create the models without loading the texture
    for (const auto& [name, ID] : block_id_name_data_) {
        auto tokens = Tokenize(name, ':');

        std::unique_ptr<ModelV2::BlockModelV2> model = getBlockModel("block/" + tokens.back(), tokens.front());
        model->is_initialized_ = true;
        GetBlockType(ID)->block_model_data_ = std::move(model);
    }

    //Bake all of the texture variables next

    for (auto& block : block_type_data_) {
        if (block->block_model_data_ == NULL) continue;
        block->block_model_data_->FlattenVariables();
    }

    //Bake all rotations into the UV
    for (auto& block : block_type_data_) {
        if (block->block_model_data_ == NULL) continue;
        block->block_model_data_->BakeTextureRotation();
    }

    //Generate all the textures now
    for (auto& block  : block_type_data_) {
        if (block->block_model_data_ == NULL) continue;

        for (auto& element : block->block_model_data_->elements_) {
            for (int i = 0; i < 6; i++) {
                const  std::string& path = element.faces_[i].reference_texture_;
                if (path.length() == 0) continue;

                //Check if it is loaded already
                if (textureIds.count(path)) {
                    element.faces_[i].texture_id_ = textureIds[path];
                    element.faces_[i].texture_count_ = textureRepeatCount[element.faces_[i].texture_id_];
                    element.faces_[i].has_transparency_ = textureTransparency[element.faces_[i].texture_id_];
                    element.faces_[i].is_see_through_ = textureSeeThrough[element.faces_[i].texture_id_];
                    continue; //Exits
                }

                auto Tokens = Tokenize(path, ':');

                //Load texture
                std::string TexFile = "assets/" + Tokens.back() + "/textures/" + Tokens.front() + ".png";

                size_t numLayersBefore = block_texture_atlas_.GetBlockCount();

                // glm::vec4 uv{};

                bool transparency = false;
                bool isSeeThrough = false;

                std::optional<RawTextureData> d = block_texture_atlas_.AddTextureToAtlas(TexFile, transparency, isSeeThrough);

                if (!d.has_value()) {
                    g_logger.LogError("Texture Loading", "Unable to load texture");
                    continue;
                }

                textureIds[path] = numLayersBefore + 1;
                textureRepeatCount[numLayersBefore + 1] = block_texture_atlas_.GetBlockCount() - numLayersBefore;
                textureTransparency[numLayersBefore + 1] = transparency;
                textureSeeThrough[numLayersBefore + 1] = isSeeThrough;


                element.faces_[i].texture_id_ = textureIds[path];
                element.faces_[i].texture_count_ = textureRepeatCount[element.faces_[i].texture_id_];
                element.faces_[i].has_transparency_ = transparency;
                element.faces_[i].is_see_through_ = isSeeThrough;
            }
        }
    }


    for (int i = 0; i < block_type_data_.size(); i++) {
        ModelV2::BlockModelV2 model;

        if (block_type_data_[i]->block_model_data_ != NULL) {
            model = *block_type_data_[i]->block_model_data_;
        }
        model.texture_variable_.clear();
        block_model_data_.emplace_back(model);
    }
}
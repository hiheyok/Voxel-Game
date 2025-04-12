#include <filesystem>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

#include "Level/Block/Blocks.h"
#include "Level/Block/Type/BlockTypes.h"
#include "FileManager/Files.h"
#include "RenderEngine/BlockModel/ModelLoader.h"
#include "RenderEngine/BlockModel/BlockModels.h"
#include "RenderEngine/OpenGL/Texture/Types/TextureAtlas.h"

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

    FastHashMap<std::string, size_t> textureIds;
    FastHashMap<size_t, size_t> textureRepeatCount;
    FastHashMap<size_t, bool> partialTransparency;
    FastHashMap<size_t, bool> fullyTransparent;
 
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
    for (auto& block  : block_type_data_) {
        if (block->block_model_data_ == 0) continue;

        for (auto& element : block->block_model_data_->elements_) {
            for (int i = 0; i < 6; i++) {
                const std::string& path = element.faces_[i].reference_texture_;
                if (path.length() == 0) continue;

                //Check if it is loaded already
                if (textureIds.count(path)) {
                    element.faces_[i].texture_id_ = textureIds[path];
                    element.faces_[i].texture_count_ = textureRepeatCount[element.faces_[i].texture_id_];
                    element.faces_[i].partially_transparent_pixel_ = partialTransparency[element.faces_[i].texture_id_];
                    element.faces_[i].fully_transparent_pixel_ = fullyTransparent[element.faces_[i].texture_id_];
                    continue; //Exits
                }

                auto Tokens = Tokenize(path, ':');

                //Load texture
                std::string TexFile = "assets/" + Tokens.back() + "/textures/" + Tokens.front() + ".png";
                size_t numLayersBefore = block_texture_atlas_->GetBlockCount();

                std::optional<RawTextureData> d = block_texture_atlas_->AddTextureToAtlas(TexFile);

                if (!d.has_value()) {
                    g_logger.LogWarn("BlockList::InitializeBlockModel", "Unable to load texture");
                    continue;
                }

                bool partial = d->HasPartiallyTransparentPixels();
                bool full = d->HasFullyTransparentPixels();

                textureIds[path] = numLayersBefore + 1;
                textureRepeatCount[numLayersBefore + 1] = block_texture_atlas_->GetBlockCount() - numLayersBefore;
                partialTransparency[numLayersBefore + 1] = partial;
                fullyTransparent[numLayersBefore + 1] = full;


                element.faces_[i].texture_id_ = textureIds[path];
                element.faces_[i].texture_count_ = textureRepeatCount[element.faces_[i].texture_id_];
                element.faces_[i].partially_transparent_pixel_ = partial;
                element.faces_[i].fully_transparent_pixel_ = full;
            }
        }
    }


    for (size_t i = 0; i < block_type_data_.size(); i++) {
        BlockModel model;

        if (block_type_data_[i]->block_model_data_ != 0) {
            model = *block_type_data_[i]->block_model_data_;
        }
        model.texture_variable_.clear();
        block_model_data_.emplace_back(model);
    }
}

void BlockList::Initialize() {
    block_texture_atlas_ = std::make_unique<TextureAtlas>();
    block_texture_atlas_->SetSize(16 * 512, 16 * 512);

    InitializeBlockModels();

    block_texture_atlas_->UploadToGPU();
}

BlockList::BlockList() : block_texture_atlas_{ std::make_unique<TextureAtlas>() } {

}

BlockList::~BlockList() {
    CleanUp();
}

void BlockList::CleanUp() {
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
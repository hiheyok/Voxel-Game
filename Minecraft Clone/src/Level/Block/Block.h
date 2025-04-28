#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Core/Typenames.h"

class Dimension;
class BlockTextureAtlas;
class ModelLoader;

struct BlockModel;

struct BlockProperties {
    BlockProperties();

    bool is_fluid_ = false;
    bool transparency_ = false;
    bool is_solid_ = false;
    bool light_pass_ = false;
};

class Block {
   public:
    Block();
    ~Block();

    virtual void Tick(const BlockPos& pos, Dimension* currentWorld) = 0;

    // For client side
    virtual void InitializeBlockModel(ModelLoader& modelLoader);
    void InitializeTexture(BlockTextureAtlas& textureAtlas);

    BlockID id_ = 0;
    std::unique_ptr<BlockModel> block_model_data_;
    std::unique_ptr<BlockProperties> properties_;
    std::string block_name_ = "";
};

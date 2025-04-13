#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Level/Block/Texture/BlockTexture.h" // TODO: Remove this later
#include "Core/Typenames.h"

class Dimension;
class BlockTexture;
class BlockTextureAtlas;
class ModelLoader;

struct BlockModel;

struct BlockType {
    BlockType(bool transparent, bool solid, bool fluid);

    BlockType();

    bool is_fluid_ = false;
    bool transparency_ = false;
    bool is_solid_ = false;
};

class Block {
public:
    Block();
    ~Block();

    virtual void InitializeBlockModel(ModelLoader& modelLoader);
    virtual void Tick(const BlockPos& pos, Dimension* currentWorld) = 0;

    void InitializeTexture(BlockTextureAtlas& textureAtlas);

    BlockID id_ = 0;
    std::unique_ptr<BlockModel> block_model_data_;
    std::unique_ptr<BlockTexture> texture_;
    std::unique_ptr<BlockType> properties_;

    std::string block_name_ = "";
};

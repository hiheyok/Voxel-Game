#pragma once
#include "../Chunk/Block/Blocks.h"
#include "../Typenames.h"

#include <string>

struct ItemProperties {
    int max_stack_ = 64;
    bool is_block_item_ = false;
    BlockID block_ = NULL;
    ItemID id_ = NULL;
    std::string name_ = "";
};

class Item {
public:
    Item() {

    }

    BlockID GetBlock() const;

    RawTextureData GetTexture() const;

    bool CheckIsBlockItem() const;

    ItemProperties properties_;
    
private:

};


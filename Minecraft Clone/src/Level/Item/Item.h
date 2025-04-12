#pragma once
#include <string>

#include "Core/Typenames.h"

class RawTextureData;

struct ItemProperties {
    int max_stack_ = 64;
    bool is_block_item_ = false;
    BlockID block_ = 0;
    ItemID id_ = 0;
    std::string name_ = "";
};

class Item {
public:
    Item() {

    }

    BlockID GetBlock() const;

    bool CheckIsBlockItem() const;

    ItemProperties properties_;
    
private:

};


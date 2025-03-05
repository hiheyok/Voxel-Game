#pragma once
#include "../Type/EntityType.h"

class Player : public Entity {
public:
    EntityInventory entity_inventory_;

    Player() {
        properties_.is_chunk_loader_ = true;
    }

private:
};
#pragma once
#include "Level/Entity/Entity.h"
#include "Level/Item/Inventory/EntityInventory.h"

class Player : public Entity {
public:
    EntityInventory entity_inventory_;

    Player() {
        properties_.is_chunk_loader_ = true;
    }

private:
};
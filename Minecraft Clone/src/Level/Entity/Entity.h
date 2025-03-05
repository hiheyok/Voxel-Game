#pragma once
#include "Properties/EntityProperties.h"
#include "../Typenames.h"
#include "../Item/Inventory/EntityInventory.h"

extern int AmountEntityDestroy;

struct Entity  {
    
    EntityProperty properties_; //Provide properties such as position, acceleration, etc.

    bool is_dirty_ = true;
    int dimension_id_ = NULL; //provide dimension location

    void Tick();
};

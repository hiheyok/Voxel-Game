#pragma once
#include "Properties/EntityProperties.h"
#include "../Typenames.h"

class Dimension;

struct Entity  {
    
    EntityProperty properties_; //Provide properties such as position, acceleration, etc.

    bool is_dirty_ = true;

    Entity();
    void Tick(Dimension* dimension);
};

#pragma once
#include "Properties/EntityProperties.h"
#include "../Typenames.h"

class Dimension;

struct Entity  {
    Entity();

    EntityProperty properties_; //Provide properties such as position, acceleration, etc.

    bool is_dirty_ = true;

    void Tick(Dimension* dimension);
};

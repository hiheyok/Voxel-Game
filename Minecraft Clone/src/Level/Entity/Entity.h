#pragma once
#include "Level/Entity/Properties/EntityProperties.h"
#include "Core/Typenames.h"

class Dimension;

struct Entity  {
    Entity();

    EntityProperty properties_; //Provide properties such as position, acceleration, etc.

    bool is_dirty_ = true;

    void Tick(Dimension* dimension);
};

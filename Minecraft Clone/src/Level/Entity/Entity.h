#pragma once
#include "Core/Typenames.h"
#include "Level/Entity/Properties/EntityProperties.h"

class Dimension;

struct Entity {
    Entity();

    EntityProperty
        properties_;  // Provide properties such as position, acceleration, etc.

    bool is_dirty_ = true;

    void Tick(Dimension* dimension);
};

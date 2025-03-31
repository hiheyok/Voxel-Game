#pragma once
#include "EntityType.h"

class Entity;
class Dimension;

class FallingBlock : public EntityType {
    void Tick(Entity* entity, Dimension* dimension) override;
};
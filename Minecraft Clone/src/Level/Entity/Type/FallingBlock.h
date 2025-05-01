#pragma once
#include "Level/Entity/Type/EntityType.h"

class Dimension;

struct Entity;

class FallingBlock : public EntityType {
  void Tick(Entity* entity, Dimension* dimension) override;
};

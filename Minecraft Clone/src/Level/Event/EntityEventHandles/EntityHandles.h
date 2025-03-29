#pragma once
#include "../../Entity/Entity.h"
#include "../../Event/Event.h"

class Dimension;

void HandleEntityTick(const Event::EntityEvent& e, Dimension* dimension);
void HandleEntitySummon(const Event::EntityEvent& e, Dimension* dimension);
void HandleRemoveEntity(const Event::EntityEvent& e, Dimension* dimension);
#pragma once
#include "../../Entity/Entity.h"
#include "../../Event/Event.h"

void HandleEntityTick(Event::EntityEvent e);
void HandleEntitySummon(Event::EntityEvent e);
void HandleRemoveEntity(Event::EntityEvent e);
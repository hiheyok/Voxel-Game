#pragma once
#include "../../Entity/Entity.h"
#include "../../Event/Event.h"

void HandleEntityTick(Event::EventDataType::_EntityEvent e);
void HandleEntitySummon(Event::EventDataType::_EntityEvent e);
void HandleRemoveEntity(Event::EventDataType::_EntityEvent e);
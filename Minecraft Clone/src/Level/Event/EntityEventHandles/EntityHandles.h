#pragma once

class Dimension;

struct EntityEvent;

void HandleEntityTick(const EntityEvent& e, Dimension* dimension);
void HandleEntitySummon(const EntityEvent& e, Dimension* dimension);
void HandleRemoveEntity(const EntityEvent& e, Dimension* dimension);

#include "EntityHandles.h"
#include "../../Event/Event.h"
#include "../../Entity/Entity.h"
#include "../../Entity/Entities.h"
#include "../../Chunk/Block/Block.h"
#include "../../../Level/Dimension/Dimension.h"

void HandleEntityTick(const EntityEvent& e, Dimension* dimension) { //prob useless
    Entity* entity = dimension->world_interactions_.GetEntity(e.entity_type_);
    entity->Tick(dimension);
}

void HandleEntitySummon(const EntityEvent& e, Dimension* dimension) {
    Entity* entity = new Entity;
    entity->properties_.position_.x = static_cast<float>(e.pos_.x);
    entity->properties_.position_.y = static_cast<float>(e.pos_.y);
    entity->properties_.position_.z = static_cast<float>(e.pos_.z);
    entity->properties_.type_ = e.entity_type_;
    dimension->world_interactions_.AddEntity(entity);
}

void HandleRemoveEntity(const EntityEvent& e, Dimension* dimension) {
    dimension->world_interactions_.KillEntity(e.entity_uuid_);
}
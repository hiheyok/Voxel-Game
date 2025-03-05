#include "ServerInteraction.h"

void ServerInteraction::AssignServer(InternalServer* server) { //Default  to  integrated server for now
    server_ = server;
}

bool ServerInteraction::CheckCollusion(Ray& ray) {
    return server_->CheckRayIntersection(ray);
}
void ServerInteraction::SendEvent(Event::Event event) {
    server_->SendEvent(event);
}

BlockID ServerInteraction::GetBlock(const BlockPos& pos) {
    return server_->GetBlock(pos);
}

std::vector<EntityUUID> ServerInteraction::GetRemovedEntities() {
    return server_->GetRemovedEntities();
}

std::vector<EntityProperty> ServerInteraction::GetUpdatedEntities() {
    return server_->GetUpdatedEntities();
}
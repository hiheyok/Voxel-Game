#pragma once
#include "../../Level/Server/Communication/InternalServer.h"

//Abstraction to interact with server
//For external server it will  store a cache of the world
class ServerInteraction {
public:
    void AssignServer(InternalServer* server);
    bool CheckCollusion(Ray& ray);
    void SendEvent(Event::Event event);
    BlockID GetBlock(const BlockPos& pos);
    std::vector<EntityUUID> GetRemovedEntities();
    std::vector<EntityProperty> GetUpdatedEntities();
private:
    int dimension_id_ = 0;
    InternalServer* server_;
};
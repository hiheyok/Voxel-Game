#pragma once
#include "../../Level/Server/Communication/InternalServer.h"

//Abstraction to interact with server
//For external server it will  store a cache of the world
class ServerInteraction {
public:
	void AssignServer(InternalServer*  pServerIn) { //Default  to  integrated server for now
		server = pServerIn;
	}

	bool checkCollusion(Ray& ray) {
		return server->CheckRayIntersection(ray);
	}
	void SendEvent(Event pEventIn) {
		server->SendEvent(pEventIn);
	}

	BlockID GetBlock(int x, int y, int z) {
		return server->GetBlock(x, y, z);
	}

	std::vector<EntityUUID> getRemovedEntities() {
		return server->getRemovedEntities();
	}

	std::vector<EntityProperty> getUpdatedEntities() {
		return server->getUpdatedEntities();
	}

private:
	int DimensionID = 0;
	InternalServer* server;
};
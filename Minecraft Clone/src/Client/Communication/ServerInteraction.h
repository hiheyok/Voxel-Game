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
		return server->checkRayIntersection(ray);
	}
	void sendEvent(Event pEventIn) {
		server->sendEvent(pEventIn);
	}

	BlockID getBlock(int x, int y, int z) {
		return server->getBlock(x, y, z);
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
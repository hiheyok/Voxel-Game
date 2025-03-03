#pragma once
#include "../../Level/Server/Communication/InternalServer.h"

//Abstraction to interact with server
//For external server it will  store a cache of the world
class ServerInteraction {
public:
	void AssignServer(InternalServer*  server) { //Default  to  integrated server for now
		server_ = server;
	}

	bool CheckCollusion(Ray& ray) {
		return server_->CheckRayIntersection(ray);
	}
	void SendEvent(Event::Event event) {
		server_->SendEvent(event);
	}

	BlockID GetBlock(const BlockPos& pos) {
		return server_->GetBlock(pos);
	}

	std::vector<EntityUUID> GetRemovedEntities() {
		return server_->GetRemovedEntities();
	}

	std::vector<EntityProperty> GetUpdatedEntities() {
		return server_->GetUpdatedEntities();
	}

private:
	int dimension_id_ = 0;
	InternalServer* server_;
};
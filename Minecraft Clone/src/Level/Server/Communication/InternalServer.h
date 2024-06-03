#pragma once
#include "../Server.h"

class InternalServer {
private:
	Server* server;
public:

	void startInternalServer(int genThreads) {
		server = new Server();
		ServerSettings settings;
		settings.genThreadCount = genThreads;

		server->startServer(settings);
	}

	Chunk* getUpdatedChunk() {
		std::vector<ChunkID> updatedChunkIDs = server->getUpdatedChunkIDs();
	}

};
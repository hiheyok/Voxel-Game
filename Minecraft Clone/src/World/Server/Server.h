#pragma once

#include "../World.h"
#include <thread>


enum ServerType
{
	INTEGRATED_SERVER, DEDICATED_SERVER
};

struct ServerSettings {
	int V_RenderDistance = 0; //Vertical
	int H_RenderDistance = 0; //Horizontal
};

class Server {
public:
	void Start(ServerSettings settings);
	void Stop();

	void ServerLoop();

	size_t genThreads = 16;

	World* world = nullptr;
private:

	void CreateWorld();

	bool stop = false;

	ServerType type;

	std::thread mainThread;
};
#pragma once

#include "../World.h"
#include <thread>

const float TPS = 20.f;

enum ServerType
{
	INTEGRATED_SERVER, DEDICATED_SERVER
};

struct ServerSettings {
	int V_RenderDistance = 0; //Vertical
	int H_RenderDistance = 0; //Horizontal
	int genThreads = 0;
};

class Server {
public:
	void Start(ServerSettings settings);
	void Stop();
	void Join();
	void SendPlayerEvent(Event event);

	void ServerLoop();

	ServerSettings settings;

	World* world = nullptr;
private:

	void CreateWorld();

	bool stop = false;

	ServerType type;

	std::thread mainThread;
};
#include "Server.h"

#include "../../Utils/LogUtils.h"

#include <thread>
#include <chrono>
#include <Windows.h>

void Server::Start(ServerSettings serverSettings) {
	Logger.LogInfo("Server", "Starting");

	if (world == nullptr) {
		world = new World;
	}

	WorldSettings worldSettings;
	worldSettings.genThreads = serverSettings.genThreads;
	world->H_RenderDistance = serverSettings.H_RenderDistance;
	world->V_RenderDistance = serverSettings.V_RenderDistance;

	settings = serverSettings;

	world->Start(worldSettings);

	type = INTEGRATED_SERVER; 

	mainThread = std::thread(&Server::ServerLoop, this);
}

void Server::Stop() {
	world->Stop();
	stop = true;
	mainThread.join();
}

void Server::ServerLoop() {

	Logger.LogInfo("Server", "Starting server main thread");

	while (!stop) {
		stime.Set();
		Timer time;

		world->Tick();
		world->Load();
		
		int MSPT = time.GetTimePassed_ms();

		if (MSPT < (1000.f / TPS)) {
			timeBeginPeriod(1);
			std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.f / TPS) - MSPT));
			timeEndPeriod(1);
		}
	//	Logger.LogInfo("Server", "MSPT: " + time.StrGetTimePassed_ms());
		
	}
}

void Server::Join() {

}

void Server::SendPlayerEvent(Event event) {
	world->QueueEvent(event);
}
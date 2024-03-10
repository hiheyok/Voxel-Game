#pragma once
#include "../../World/World.h"
#include "MultiEntityRender.h"
#include "../../World/Server/Time/Timer.h"
#include <thread>

class EntityRendererUpdater {
	MultiEntityRenderer* renderer;

	bool stop = true;

	void UpdaterThread();

	std::thread UpdateThread;

	Timer* ServerTime;

public:

	void SetEntityRenderer(MultiEntityRenderer* render, Timer* time);
	void Start();
	void Stop();
};
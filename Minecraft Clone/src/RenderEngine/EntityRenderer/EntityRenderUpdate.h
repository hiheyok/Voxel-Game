#pragma once
#include "MultiEntityRender.h"
#include "../../Level/Timer/Timer.h"
#include <thread>

class EntityRendererUpdater {
	MultiEntityRenderer* renderer_;

	bool stop = true; // TODO: Rename this

	void UpdaterThread();

	std::thread update_thread_;

	Timer* server_time_;

public:

	void SetEntityRenderer(MultiEntityRenderer* render, Timer* time);
	void Start();
	void Stop();
};
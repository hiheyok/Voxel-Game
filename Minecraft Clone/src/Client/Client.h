#pragma once
#include "../RenderEngine/Window.h"
#include "../RenderEngine/Camera/camera.h"
#include "../World/World.h"
#include "Render/WorldRender.h"
#include "LocalWorld/LocalWorld.h"
class Client : protected Window {
public:
	void run();
private:

	void GameLoop();
	void Update();
	void Cleanup();

	double Frametime = 0.5;

	LocalWorld MainLocalWorld;

	World MainWorld;
	WorldRender TerrainRender;
};
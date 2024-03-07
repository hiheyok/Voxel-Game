#pragma once
#include "../RenderEngine/Window.h"
#include "../RenderEngine/Camera/camera.h"
#include "../World/Server/Server.h"
#include "Render/WorldRender.h"
#include "LocalWorld/LocalWorld.h"
#include "../RenderEngine/EntityRenderer/MultiEntityRender.h"
#include "../RenderEngine/OpenGL/Framebuffer/Framebuffer.h"

class Client : protected Window {
public:
	void run();
private:

	void GameLoop();
	void Update();
	void Cleanup();

	void SetWindowName();

	double Frametime = 0.5;

	LocalWorld MainLocalWorld;

	Server server;
	WorldRender TerrainRender;
	MultiEntityRenderer EntityRender;
	TexturedFrameBuffer Framebuffer;
	TexturedFrameBuffer FramebufferSuperres;
	
};
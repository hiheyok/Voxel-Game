#pragma once
#include "../RenderEngine/Window.h"
#include "../RenderEngine/Camera/camera.h"
#include "../World/Server/Server.h"
#include "Render/WorldRender.h"
#include "../RenderEngine/EntityRenderer/MultiEntityRender.h"
#include "../RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "Player/MainPlayer.h"

class Client : protected Window {
public:
	void run();
private:
	void Initialize();
	void GameLoop();
	void Update();
	void Cleanup();

	void SetWindowName();

	double Frametime = 0.5;

	MainPlayer m_MainPlayer;

	Server server;
	WorldRender TerrainRender;
	MultiEntityRenderer EntityRender;
	TexturedFrameBuffer Framebuffer;
	
};
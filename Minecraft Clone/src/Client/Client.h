#pragma once
#include "../RenderEngine/Window.h"
#include "Render/WorldRender.h"
#include "../RenderEngine/EntityRenderer/MultiEntityRender.h"
#include "../RenderEngine/EntityRenderer/EntityRenderUpdate.h"`
#include "../RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "Player/MainPlayer.h"
#include "../RenderEngine/GUI/GUI.h"
#include "../Level/Item/ItemTextureAtlas.h"
#include "Render/DebugScreen/DebugScreen.h"
#include "../Level/Server/Communication/InternalServer.h"

class Client : protected Window {
public:
	void run();
private:
	void Initialize();
	void GameLoop();
	void Update();
	void Cleanup();
	void Render();
	void SetDebugScreen();

	double Frametime = 0.5;

	MainPlayer m_MainPlayer;

	InternalServer server;
	WorldRender TerrainRender;
	MultiEntityRenderer EntityRender;
	EntityRendererUpdater EntityUpdater;
	TexturedFrameBuffer Framebuffer;
	TextRenderer TextRender;
	DebugScreen debugScreen;
};
#pragma once
#include "../RenderEngine/Window.h"
#include "Render/WorldRender.h"
#include "../RenderEngine/EntityRenderer/MultiEntityRender.h"
#include "../RenderEngine/EntityRenderer/EntityRenderUpdate.h"
#include "../RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "Player/MainPlayer.h"
#include "../RenderEngine/GUI/GUI.h"
#include "../Level/Item/ItemTextureAtlas.h"
#include "Render/DebugScreen/DebugScreen.h"
#include "../Level/Server/Communication/InternalServer.h"
#include "Profiler/PerformanceProfiler.h"

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

	double frametime_ = 0.5;

	MainPlayer main_player_;

	InternalServer server_;
	WorldRender terrain_render_;
	MultiEntityRenderer entity_render_;
	EntityRendererUpdater entity_updater_;
	TexturedFrameBuffer framebuffer_;
	TextRenderer text_render_;
	DebugScreen debug_screen_;
	PerformanceProfiler profiler_;
};
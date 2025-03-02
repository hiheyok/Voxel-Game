#include "Client.h"
#include "../Utils/LogUtils.h"
#include "../Level/Chunk/Block/Blocks.h"
#include "../Level/Entity/Entities.h"
#include <chrono>
#include "../Level/Timer/Timer.h"
#include "../Core/Options/Option.h"

using namespace std;
using namespace chrono;
using namespace glm;

void Client::Initialize() {
	Start();

	Blocks.Initialize();
	g_entity_list.Initialize();
	EntityRender.Initialize(&profiler);
	EntityRender.SetWindow(getWindow());
	TextRender.InitializeTextRenderer(getWindow());
	g_items.RegisterAll();
	g_item_atlas.Initialize(512*16 * 2, 16*2*8);

	for (auto& item : g_items.item_container_) {
		g_item_atlas.AddItem(item.second);
	}

	Framebuffer.genBuffer(Properties.WindowSizeX, Properties.WindowSizeY, (float)AppOptions.GraphicsScale, GL_RGB);

	DisableCursor();

	server.startInternalServer(&main_player_.m_Player);

	main_player_.Initialize(getWindow(), &server);
	main_player_.SetPlayerPosition(0., 50, 0.);
	main_player_.SetPlayerRotation(-135.f, -30.);

	EntityUpdater.SetEntityRenderer(&EntityRender, server.getTickClock());

	EntityUpdater.Start();

	Logger.LogInfo("World", "Generating World");
	TerrainRender.Start(getWindow(), &server, &profiler);
	Logger.LogInfo("Client", "Starting Gameloop");

	debugScreen.Initialize(getWindow());
}

void Client::run() {
	Initialize();
	GameLoop();
	Cleanup();
}

void Client::Cleanup() {
	EntityRender.Clean();
	TerrainRender.Stop();
	server.Stop();
	Logger.Stop();
	EntityUpdater.Stop();
	Logger.LoggingThread.join();
	Blocks.CleanUp();
	glfwDestroyWindow(getWindow());
}

void Client::SetDebugScreen() {
	debugScreen.EditText("Stat1", "VRAM Usage: " + to_string((double)TerrainRender.renderer_v2_.getVRAMUsageFull() / 1000000.0) + " MB");
	debugScreen.EditText("Stat2", "XYZ: " + to_string(main_player_.GetEntityProperties().position_.x) + "/" + to_string(main_player_.GetEntityProperties().position_.y) + "/" + std::to_string(main_player_.GetEntityProperties().position_.z));
	debugScreen.EditText("Stat3", "Velocity XYZ: " + to_string(main_player_.GetEntityProperties().velocity_.x) + "/" + to_string(main_player_.GetEntityProperties().velocity_.y) + "/" + std::to_string(main_player_.GetEntityProperties().velocity_.z));
	debugScreen.EditText("Stat4", "VRAM Fragmentation Rate: " + to_string(TerrainRender.renderer_v2_.getFragmentationRate() * 100) + "%");
	debugScreen.EditText("Stat5", "FPS: " + to_string(1.0 / Frametime));
	debugScreen.EditText("Stat6", "Mesh Stats (ms) Total/S0/S1/S2: " + to_string(TerrainRender.buildTime / 1000.f) + "/" + to_string(TerrainRender.buildstage0 / 1000.f) + "/" + std::to_string(TerrainRender.buildstage1 / 1000.f) + "/" + std::to_string(TerrainRender.buildstage2 / 1000.f));
	debugScreen.EditText("Stat7", "Entity Count: N/A");
	debugScreen.EditText("Stat8", "Event Queue Size: N/A");
	debugScreen.EditText("Stat9", "Server Tick (MSPT): " + to_string(server.getMSPT()));
	debugScreen.EditText("Stat10","Chunk Count: " + std::to_string(server.getChunkCount()));
	debugScreen.Update();
}

void Client::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Framebuffer.bindFBO();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!Properties.DrawSolid)
		renderLines();

	EntityRender.Render();
	TerrainRender.Render();

	if (!Properties.DrawSolid)
		renderSolid();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Framebuffer.unbindFBO();

	Framebuffer.render();

	main_player_.RenderGUIs();
	debugScreen.Render();
}

void Client::GameLoop() {

	Timer time;

	while (!WindowCloseCheck()) {
		Timer FrametimeTracker;

		Update();

		Render();
		profiler.ProfileStart("root/refresh");
		Refresh();
		profiler.ProfileStop("root/refresh");
		Frametime = FrametimeTracker.GetTimePassed_s();

		Inputs.delta = Frametime;

		if (time.GetTimePassed_ms() > 50) {
			SetDebugScreen();
			time.Set();
		}
		
	}
}

void Client::Update() {
	PollInputs();

	if (Inputs.CheckKey(GLFW_KEY_F)) {
		Properties.DrawSolid = false;
	}
	if (Inputs.CheckKey(GLFW_KEY_G)) {
		Properties.DrawSolid = true;
	}

	if (Inputs.CheckKeyPress(GLFW_KEY_R)) {
		EntityRender.Reload();
	}

	if (Inputs.CheckKeyPress(GLFW_KEY_P)) {
		profiler.LoadCache();
		profiler.print();
	}

	if (Inputs.CheckKey(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(getWindow(), true);
	}



	if (Properties.WindowSizeDirty) {
		Properties.WindowSizeDirty = false;

		Framebuffer.clear();
		Framebuffer.genBuffer(Properties.WindowSizeX, Properties.WindowSizeY, (float)AppOptions.GraphicsScale);
	}

	main_player_.Update(Inputs);

	TerrainRender.SetPosition(main_player_.GetEntityProperties().position_);
	TerrainRender.SetRotation(main_player_.GetEntityProperties().rotation_);

	EntityRender.SetPosition(main_player_.GetEntityProperties().position_);
	EntityRender.SetRotation(main_player_.GetEntityProperties().rotation_);

	EntityRender.SetTimePastTick(server.getTickClock()->GetTimePassed_s());
	EntityRender.Update();

	TerrainRender.Update();

	Inputs.UpdateAllKey();
}

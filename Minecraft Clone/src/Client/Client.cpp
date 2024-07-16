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
	EntityList.Initialize();
	EntityRender.Initialize();
	EntityRender.SetWindow(getWindow());
	TextRender.InitializeTextRenderer(getWindow());
	Items.REGISTER_ALL();
	ItemAtlas.Initialize(196*16  * 2, 16*4);
	for (auto& item : Items.ItemContainer) {
		ItemAtlas.AddItem(item.second);
	}

	Framebuffer.genBuffer(Properties.WindowSizeX, Properties.WindowSizeY, (float)AppOptions.GraphicsScale, GL_RGB);

	DisableCursor();

	server.startInternalServer(&m_MainPlayer.m_Player);

	m_MainPlayer.Initialize(getWindow(), &server);
	m_MainPlayer.SetPlayerPosition(0., 10, 0.);
	m_MainPlayer.SetPlayerRotation(-135.f, -30.);

	EntityUpdater.SetEntityRenderer(&EntityRender, server.getTickClock());

	EntityUpdater.Start();
	

	Logger.LogInfo("World", "Generating World");
	TerrainRender.Start(getWindow(), &server);
	Logger.LogInfo("Client", "Starting Gameloop");

	debugScreen.Initialize(getWindow());
}

void Client::run() {
	Initialize();
	GameLoop();
	Cleanup();
}

void Client::Cleanup() {
	EntityRender.clean();
	TerrainRender.Stop();
	server.Stop();
	Logger.Stop();
	EntityUpdater.Stop();
	Logger.LoggingThread.join();
	Blocks.CleanUp();
	glfwDestroyWindow(getWindow());
}

void Client::SetDebugScreen() {
	debugScreen.EditText("Stat1", "VRAM Usage: " + to_string((double)TerrainRender.RendererV2.getVRAMUsageFull() / 1000000.0) + " MB");
	debugScreen.EditText("Stat2", "XYZ: " + to_string(m_MainPlayer.GetEntityProperties().Position.x) + "/" + to_string(m_MainPlayer.GetEntityProperties().Position.y) + "/" + std::to_string(m_MainPlayer.GetEntityProperties().Position.z));
	debugScreen.EditText("Stat3", "Velocity XYZ: " + to_string(m_MainPlayer.GetEntityProperties().Velocity.x) + "/" + to_string(m_MainPlayer.GetEntityProperties().Velocity.y) + "/" + std::to_string(m_MainPlayer.GetEntityProperties().Velocity.z));
	debugScreen.EditText("Stat4", "VRAM Fragmentation Rate: " + to_string(TerrainRender.RendererV2.getFragmentationRate() * 100) + "%");
	debugScreen.EditText("Stat5", "FPS: " + to_string(1.0 / Frametime));
	debugScreen.EditText("Stat6", "Mesh Stats (ms) Total/S0/S1/S2: " + to_string(TerrainRender.buildTime / 1000.f) + "/" + to_string(TerrainRender.buildstage0 / 1000.f) + "/" + std::to_string(TerrainRender.buildstage1 / 1000.f) + "/" + std::to_string(TerrainRender.buildstage2 / 1000.f));
	debugScreen.EditText("Stat7", "Entity Count: N/A");
	debugScreen.EditText("Stat8", "Event Queue Size: N/A");
	debugScreen.EditText("Stat9", "Server Tick (MSPT): " + to_string(server.getMSPT()));
	debugScreen.EditText("Stat10", "Chunk Count: " + std::to_string(server.getChunkCount()));
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
	
	m_MainPlayer.RenderGUIs();
	debugScreen.Render();
}

void Client::GameLoop() {

	Timer time;

	while (!WindowCloseCheck()) {
		Timer FrametimeTracker;

		Update();

		Render();

		Refresh();

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

	if (Inputs.CheckKey(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(getWindow(), true);
	}



	if (Properties.WindowSizeDirty) {
		Properties.WindowSizeDirty = false;

		Framebuffer.clear();
		Framebuffer.genBuffer(Properties.WindowSizeX, Properties.WindowSizeY, (float)AppOptions.GraphicsScale);
	}

	m_MainPlayer.Update(Inputs);

	TerrainRender.SetPosition(m_MainPlayer.GetEntityProperties().Position);
	TerrainRender.SetRotation(m_MainPlayer.GetEntityProperties().Rotation);

	EntityRender.SetPosition(m_MainPlayer.GetEntityProperties().Position);
	EntityRender.SetRotation(m_MainPlayer.GetEntityProperties().Rotation);

	EntityRender.SetTimePastTick(server.getTickClock()->GetTimePassed_s());
	EntityRender.Update();

	TerrainRender.Update();

	Inputs.UpdateAllKey();
}

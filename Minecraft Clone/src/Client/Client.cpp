#include "Client.h"
#include "../Utils/LogUtils.h"
#include "../World/Chunk/Block/Blocks.h"
#include "../World/Entity/Entities.h"
#include <chrono>
#include "../World/Server/Time/Timer.h"
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

	ItemAtlas.Initialize(196*16, 16*8);
	for (auto& item : Items.ItemContainer) {
		ItemAtlas.AddItem(item.second);
	}

	Framebuffer.genBuffer(Properties.WindowSizeX, Properties.WindowSizeY, (float)AppOptions.GraphicsScale, GL_RGB);

	DisableCursor();

	m_MainPlayer.Initialize(getWindow());
	m_MainPlayer.SetPlayerPosition(0., 80., 0.);
	m_MainPlayer.SetPlayerRotation(-135.f, -30.);

	ServerSettings serverSettings;

	server.Start(serverSettings);

	EntityUpdater.SetEntityRenderer(&EntityRender, &server.stime);

	EntityUpdater.Start();

	Logger.LogInfo("World", "Generating World");
	TerrainRender.Start(getWindow(), server.world);
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
	debugScreen.EditText("VRAMUsage", "VRAM Usage: " + to_string((double)TerrainRender.RendererV2.getVRAMUsageFull() / 1000000.0) + " MB");
	debugScreen.EditText("Position", "XYZ: " + to_string(m_MainPlayer.GetEntityProperties().Position.x) + "/" + to_string(m_MainPlayer.GetEntityProperties().Position.y) + "/" + std::to_string(m_MainPlayer.GetEntityProperties().Position.z));
	debugScreen.EditText("VRAMFragmentationRate", "VRAM Fragmentation Rate: " + to_string(TerrainRender.RendererV2.getFragmentationRate() * 100) + "%");
	debugScreen.EditText("FPS", "FPS: " + to_string(1.0 / Frametime));
	debugScreen.EditText("MeshStats", "Mesh Stats (ms) Total/S0/S1/S2: " + to_string(TerrainRender.buildTime / 1000.f) + "/" + to_string(TerrainRender.buildstage0 / 1000.f) + "/" + std::to_string(TerrainRender.buildstage1 / 1000.f) + "/" + std::to_string(TerrainRender.buildstage2 / 1000.f));
	debugScreen.EditText("EntityCount", "Entity Count: " + to_string(((World*)Block::WorldPTR)->EntityData.GetEntityCount()));
	debugScreen.EditText("EventQueueSize", "Event Queue Size: " + to_string(((World*)Block::WorldPTR)->EventManager.getSize()));
	debugScreen.EditText("ServerTick", "Server Tick (MSPT): " + to_string(((World*)Block::WorldPTR)->MSPT));
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

		if (time.GetTimePassed_ms() > 250) {
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

	server.world->SetPlayerPos(m_MainPlayer.GetEntityProperties().Position);
	TerrainRender.SetPosition(m_MainPlayer.GetEntityProperties().Position);
	TerrainRender.SetRotation(m_MainPlayer.GetEntityProperties().Rotation);

	EntityRender.SetPosition(m_MainPlayer.GetEntityProperties().Position);
	EntityRender.SetRotation(m_MainPlayer.GetEntityProperties().Rotation);

	EntityRender.SetTimePastTick(server.stime.GetTimePassed_s());
	EntityRender.Update();

	TerrainRender.Update();

	Inputs.UpdateAllKey();
}

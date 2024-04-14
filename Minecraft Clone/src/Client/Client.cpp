#include "Client.h"
#include "../Utils/LogUtils.h"
#include "../World/Chunk/Block/Blocks.h"
#include "../World/Entity/Entities.h"
#include <chrono>
#include "../World/Server/Time/Timer.h"

using namespace std;
using namespace chrono;
using namespace glm;

void Client::Initialize() {
	Start();

	Blocks.Initialize();
	EntityList.Initialize();
	EntityRender.Initialize();
	EntityRender.SetWindow(getWindow());

	ItemAtlas.Initialize(256*8, 32*8);
	for (auto& item : Items.ItemContainer) {
		ItemAtlas.AddItem(item.second);
	}

	Framebuffer.genBuffer(Properties.WindowSizeX, Properties.WindowSizeY, 2.f, GL_RGB);

	DisableCursor();

	m_MainPlayer.Initialize(getWindow());
	m_MainPlayer.SetPlayerPosition(0., 80., 0.);
	m_MainPlayer.SetPlayerRotation(-135.f, -30.);

	ServerSettings serverSettings;
	serverSettings.H_RenderDistance = 32;
	serverSettings.V_RenderDistance = 32;
	serverSettings.genThreads = 6;

	server.Start(serverSettings);

	EntityUpdater.SetEntityRenderer(&EntityRender, &server.stime);

	EntityUpdater.Start();

	Logger.LogInfo("World", "Generating World");
	TerrainRender.HorizontalRenderDistance = 32;
	TerrainRender.VerticalRenderDistance = 32;
	TerrainRender.Start(getWindow(), server.world, 6);
	Logger.LogInfo("Client", "Starting Gameloop");

	
	
	
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

void Client::SetWindowName() {
	UpdateWindowName(
		"FPS: " + std::to_string(1.0 / Frametime)
		+ " | Pos: " + std::to_string(m_MainPlayer.GetEntityProperties().Position.x) + ", "
		+ std::to_string(m_MainPlayer.GetEntityProperties().Position.y) + ", "
		+ std::to_string(m_MainPlayer.GetEntityProperties().Position.z)
		+ ", VRAM Fragmentation Rate: " + std::to_string(TerrainRender.RendererV2.getFragmentationRate() * 100)
		+ ", VRAM Usage (MB): " + std::to_string((double)TerrainRender.RendererV2.getVRAMUsageFull() / 1000000.0)
		//+ " | Mesh All (ms): " + std::to_string(TerrainRender.buildTime / 1000.f)
		//+ " | S0 (ms): " + std::to_string(TerrainRender.buildstage0 / 1000.f)
		//+ " | S1 (ms): " + std::to_string(TerrainRender.buildstage1 / 1000.f)
		//+ " | S2 (ms): " + std::to_string(TerrainRender.buildstage2 / 1000.f)
	//	+ " | Total Mesh: " + std::to_string(TerrainRender.amountOfMeshGenerated)
		+ ", Entity Count: " + to_string(((World*)Block::WorldPTR)->EntityData.GetEntityCount())
		+", Event Queue Size: " + to_string(((World*)Block::WorldPTR)->EventManager.getSize())
		+ ", Server Tick (MSPT): " + to_string(((World*)Block::WorldPTR)->MSPT)
	);
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
			SetWindowName();
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

	if (Inputs.CheckKeyPress(GLFW_KEY_U)) {
		int k = 3;
		int a = k + 4;
	}

	if (Inputs.CheckKey(GLFW_KEY_Z)) {
		Logger.LogInfo("Client", "FPS: " + std::to_string(1.f / (float)Frametime));
	}

	if (Inputs.CheckKey(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(getWindow(), true);
	}



	if (Properties.WindowSizeDirty) {
		Properties.WindowSizeDirty = false;

		Framebuffer.clear();
		Framebuffer.genBuffer(Properties.WindowSizeX, Properties.WindowSizeY, 2);
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

	Inputs.Mouse.ScrollDirection = Inputs.Mouse.SCROLL_NONE;

	Inputs.Mouse.Displacement = glm::dvec2(0.0, 0.0);

	if (Inputs.Mouse.LEFT == Inputs.Mouse.PRESS) {
		Inputs.Mouse.LEFT = Inputs.Mouse.HOLD;
	}

	if (Inputs.Mouse.RIGHT == Inputs.Mouse.PRESS) {
		Inputs.Mouse.RIGHT = Inputs.Mouse.HOLD;
	}

	if (Inputs.Mouse.MIDDLE == Inputs.Mouse.PRESS) {
		Inputs.Mouse.MIDDLE = Inputs.Mouse.HOLD;
	}

}

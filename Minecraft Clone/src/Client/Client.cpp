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

	Framebuffer.genBuffer(Properties.WindowSizeX, Properties.WindowSizeY, 2);

	DisableCursor();

	m_MainPlayer.SetPlayerPosition(0., 66.0, 0.);
	m_MainPlayer.SetPlayerRotation(0., -30.);

	ServerSettings serverSettings;
	serverSettings.H_RenderDistance = 24;
	serverSettings.V_RenderDistance = 8;
	serverSettings.genThreads = 8;

	server.Start(serverSettings);

	Logger.LogInfo("World", "Generating World");
	TerrainRender.renderDistance = 24;
	TerrainRender.verticalRenderDistance = 8;
	TerrainRender.Start(getWindow(), server.world, 16);
	Logger.LogInfo("Client", "Starting Gameloop");
}

void Client::run() {
	Initialize();
	GameLoop();
	Cleanup();
}

void Client::Cleanup() {
	TerrainRender.Stop();
	server.Stop();
	Logger.Stop();
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
		/*+ " | Mesh All (ms): " + std::to_string(TerrainRender.buildTime / 1000.f)
		+ " | S0 (ms): " + std::to_string(TerrainRender.buildstage0 / 1000.f)
		+ " | S1 (ms): " + std::to_string(TerrainRender.buildstage1 / 1000.f)
		+ " | S2 (ms): " + std::to_string(TerrainRender.buildstage2 / 1000.f)
		+ " | Total Mesh: " + std::to_string(TerrainRender.amountOfMeshGenerated)*/
		+", Event Queue Size: " + to_string(((World*)Block::WorldPTR)->EventManager.getSize())
		+ ", Server Tick (MSPT): " + to_string(((World*)Block::WorldPTR)->MSPT)

	);
}

void Client::GameLoop() {

	Timer time;

	while (!WindowCloseCheck()) {
		Timer FrametimeTracker;

		Update();

		Framebuffer.bindFBO();
		Framebuffer.bindRBO();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.f);
		if (!Properties.DrawSolid) {
			renderLines();
		}

		EntityRender.Render();
		TerrainRender.Render();

		if (!Properties.DrawSolid) {
			renderSolid();
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		Framebuffer.unbindFBO();
		Framebuffer.unbindRBO();
		

		Framebuffer.render();

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

	if (Inputs.CheckKey(GLFW_KEY_R)) {
		EntityRender.Reload();
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

	server.world->EntityUpdateLock.lock();
	std::unordered_map<EntityUUID, Entity> UpdatedEntities = server.world->EntityUpdated;
	std::unordered_set<EntityUUID> RemovedEntities = server.world->RemovedEntity;
	server.world->EntityUpdated.clear();
	server.world->RemovedEntity.clear();
	server.world->EntityUpdateLock.unlock();
	
	for (auto& entity : UpdatedEntities) {
		EntityRender.AddEntity(entity.second);
	}
	for (auto& entity : RemovedEntities) {
		EntityRender.RemoveEntity(entity);
	}

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

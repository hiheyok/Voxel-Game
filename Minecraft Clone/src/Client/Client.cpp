#include "Client.h"
#include "../Utils/LogUtils.h"
#include "../World/Chunk/Block/Blocks.h"
#include "../World/Entity/Entities.h"
#include <chrono>
#include "../World/Server/Time/Timer.h"

using namespace std;
using namespace chrono;
using namespace glm;

void Client::run() {

	Start();

	Blocks.RegisterAll();
	Blocks.Initialize();
	EntityList.Initialize();
	EntityRender.Initialize();
	EntityRender.SetWindow(getWindow());

	DisableCursor();

	MainLocalWorld.SetPlayerPosition(0.,66.0,0.);
	MainLocalWorld.SetPlayerRotation(0.,-30.);

	ServerSettings serverSettings;
	serverSettings.H_RenderDistance = 32;
	serverSettings.V_RenderDistance = 8;
	serverSettings.genThreads = 8;

	server.Start(serverSettings);

	Logger.LogInfo("World", "Generating World");
	TerrainRender.renderDistance = 32;
	TerrainRender.verticalRenderDistance = 8;
	TerrainRender.Start(getWindow(), server.world, 16);

	MainLocalWorld.SetWorld(server.world);
	

	Logger.LogInfo("Client", "Starting Gameloop");
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
		+ " | Pos: " + std::to_string(MainLocalWorld.GetPlayerPosition().x) + ", "
		+ std::to_string(MainLocalWorld.GetPlayerPosition().y) + ", "
		+ std::to_string(MainLocalWorld.GetPlayerPosition().z)
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

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.f);

		TerrainRender.Render();
		
		EntityRender.Render();

		Refresh();

		Frametime = FrametimeTracker.GetTimePassed_s();

		if (time.GetTimePassed_ms() > 250) {
			SetWindowName();
			time.Set();
		}
		
	}
}

void Client::Update() {
	PollInputs();

	if (TestForKeyInputs(GLFW_KEY_F)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (TestForKeyInputs(GLFW_KEY_G)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (TestForKeyInputs(GLFW_KEY_Z)) {
		Logger.LogInfo("Client", "FPS: " + std::to_string(1.f / (float)Frametime));
	}

	if (TestForKeyInputs(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(getWindow(), true);
	}

	UpdateKeyPressSet();

	MainLocalWorld.UpdateIO(GetKeyboardDump(), TestForKeyInputs(GLFW_KEY_LEFT_SHIFT), (float)cursormovementx, (float)cursormovementy, PressedLeft, PressedMiddle, PressedRight, (float)Frametime);
	PressedLeft = false;
	PressedMiddle = false;
	PressedRight = false;

	server.world->SetPlayerPos(MainLocalWorld.GetPlayerPosition());

	cursormovementx = 0;
	cursormovementy = 0;

	TerrainRender.SetPosition(MainLocalWorld.GetPlayerPosition());
	TerrainRender.SetRotation(MainLocalWorld.GetPlayerRotation());

	EntityRender.SetPosition(MainLocalWorld.GetPlayerPosition());
	EntityRender.SetRotation(MainLocalWorld.GetPlayerRotation());

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

}

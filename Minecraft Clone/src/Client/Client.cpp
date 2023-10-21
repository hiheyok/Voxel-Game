#include "Client.h"
#include "../Utils/LogUtils.h"
#include "../World/Chunk/Block/Blocks.h"
#include "../World/Entity/Entities.h"
#include <chrono>

using namespace std;
using namespace chrono;
using namespace glm;

void Client::run() {

	Start();

	Blocks.Initialize();

	DisableCursor();

	MainLocalWorld.SetPlayerPosition(0.,60.0,0.);
	MainLocalWorld.SetPlayerRotation(0.,-30.);

	ServerSettings serverSettings;
	serverSettings.H_RenderDistance = 64;
	serverSettings.V_RenderDistance = 8;
	serverSettings.genThreads = 16;

	server.Start(serverSettings);

	Logger.LogInfo("World", "Generating World");
	TerrainRender.renderDistance = 64;
	TerrainRender.Start(getWindow(), server.world, 8);

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

	glfwDestroyWindow(getWindow());
}

void Client::GameLoop() {

	auto t2 = high_resolution_clock::now();

	while (!WindowCloseCheck()) {
		auto t0 = high_resolution_clock::now();

		Update();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		TerrainRender.Render();

		Refresh();

		Frametime = (double)(high_resolution_clock::now() - t0).count() / 1000000000.0;

		if ((high_resolution_clock::now() - t2).count() > 250000000) {
			UpdateWindowName(
				"FPS: " + std::to_string(1.0 / Frametime)
				+ " | Pos: " + std::to_string(MainLocalWorld.GetPlayerPosition().x) + ", " 
				+ std::to_string(MainLocalWorld.GetPlayerPosition().y) + ", " 
				+ std::to_string(MainLocalWorld.GetPlayerPosition().z)
				+ ", VRAM Fragmentation Rate: " + std::to_string(TerrainRender.RendererV2.getFragmentationRate() * 100) 
				+ ", VRAM Usage (MB): " + std::to_string((double)TerrainRender.RendererV2.getVRAMUsageFull() / 1000000.0)
				+ " | Mesh All (ms): " + std::to_string(TerrainRender.buildTime / 1000.f)
				+ " | S0 (ms): " + std::to_string(TerrainRender.buildstage0 / 1000.f)
				+ " | S1 (ms): " + std::to_string(TerrainRender.buildstage1 / 1000.f)
				+ " | S2 (ms): " + std::to_string(TerrainRender.buildstage2 / 1000.f)
				+ " | Total Mesh: " + std::to_string(TerrainRender.amountOfMeshGenerated)

			);
			t2 = high_resolution_clock::now();
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

	TerrainRender.Update();

}

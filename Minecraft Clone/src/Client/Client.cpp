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

	InitializeEntities();
	InitializeBlockTextures();

	DisableCursor();

	MainLocalWorld.SetPlayerPosition(0.,70.0,0.);
	MainLocalWorld.SetPlayerRotation(0.,-30.);

	MainWorld.horizontaltickingdistance = 32;

	MainWorld.Start();

	getLogger()->LogInfo("World", "Generating World");
	TerrainRender.renderDistance = 32;
	TerrainRender.Start(getWindow(), &MainWorld, 16);

	MainLocalWorld.SetWorld(&MainWorld);

	getLogger()->LogInfo("Client", "Starting Gameloop");
	GameLoop();
	Cleanup();
}

void Client::Cleanup() {
	TerrainRender.Stop();
	MainWorld.Stop();
	getLogger()->Stop();
	getLogger()->LoggingThread.join();

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
			UpdateWindowName("Voxel Engine | FPS: " + std::to_string(1.0 / Frametime) + 
				" | Pos: " + std::to_string(MainLocalWorld.GetPlayerPosition().x) + ", " +
				std::to_string(MainLocalWorld.GetPlayerPosition().y) + ", " +
				std::to_string(MainLocalWorld.GetPlayerPosition().z) + ", ");
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
		getLogger()->LogInfo("Client", "FPS: " + std::to_string(1.f / (float)Frametime));
	}

	if (TestForKeyInputs(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(getWindow(), true);
	}


	UpdateKeyPressSet();

	MainLocalWorld.UpdateIO(GetKeyboardDump(), TestForKeyInputs(GLFW_KEY_LEFT_SHIFT), (float)cursormovementx, (float)cursormovementy, PressedLeft, PressedMiddle, PressedRight, (float)Frametime);
	PressedLeft = false;
	PressedMiddle = false;
	PressedRight = false;

	MainWorld.SetPlayerPos(MainLocalWorld.GetPlayerPosition());

	cursormovementx = 0;
	cursormovementy = 0;

	TerrainRender.SetPosition(MainLocalWorld.GetPlayerPosition());
	TerrainRender.SetRotation(MainLocalWorld.GetPlayerRotation());

	TerrainRender.Update();

}

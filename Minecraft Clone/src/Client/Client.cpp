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

	MainLocalWorld.SetPlayerPosition(0.,15.0,0.);
	MainLocalWorld.SetPlayerRotation(0.,-50.);

	MainWorld.Start();

	getLogger()->LogInfo("World", "Generating World");

	TerrainRender.Start(getWindow());

	MainLocalWorld.SetWorld(&MainWorld);

	getLogger()->LogInfo("Client", "Starting Gameloop");
	GameLoop();
}

void Client::GameLoop() {
	while (!WindowCloseCheck()) {
		auto t0 = high_resolution_clock::now();

		Update();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		TerrainRender.Render();

		Refresh();

		Frametime = (double)(high_resolution_clock::now() - t0).count() / 1000000000.0;
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


	UpdateKeyPressSet();

	MainLocalWorld.UpdateIO(GetKeyboardDump(), TestForKeyInputs(GLFW_KEY_LEFT_SHIFT), (float)cursormovementx, (float)cursormovementy, PressedLeft, PressedMiddle, PressedRight, Frametime);
	PressedLeft = false;
	PressedMiddle = false;
	PressedRight = false;

	MainWorld.SetPlayerPos(MainLocalWorld.GetPlayerPosition());

	cursormovementx = 0;
	cursormovementy = 0;

	TerrainRender.SetPosition(MainLocalWorld.GetPlayerPosition());
	TerrainRender.SetRotation(MainLocalWorld.GetPlayerRotation());

	TerrainRender.PauseLoader();
	while (!MainWorld.ChunksUpdated.empty()) {
		Chunk updatingchunk;

		if (MainWorld.ChunksUpdated.try_pop(updatingchunk)) {
			if (!updatingchunk.isEmpty) {
				TerrainRender.LoadChunk(updatingchunk);
			}
		}
		
	}
	TerrainRender.UnpauseLoader();

	MainWorld.ChunksUpdated.clear();
	TerrainRender.Update();

}
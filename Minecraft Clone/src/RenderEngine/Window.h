#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include "../Utils/LogUtils.h"
#include "../Client/IO/IO.h"
#include <unordered_set>
#include <vector>

#include "../Client/IO/IO.h"

class Window {
public:
	void Start();
	void Refresh();
	void PollInputs();
	GLFWwindow* getWindow();

protected:

	bool WindowCloseCheck();
	void DisableCursor();
	void EnableCursor();
	void onMouseButton(int button, int action);
	void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void UpdateWindowName(std::string name);

	void renderSolid();
	void renderLines();
	void mouse_callback(double xpos, double ypos);

	int sizex = 1280;
	int sizey = 720;

	bool WindowSizeDirty = false;
	bool DrawSolid = true;

	UserInputs Inputs;

private:
	static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);

	void resize_window(int x, int y);
	GLFWwindow* window = nullptr;
	bool init = false;
};
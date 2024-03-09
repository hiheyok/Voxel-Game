#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include "../Utils/LogUtils.h"
#include "../Client/IO/IO.h"
#include <unordered_set>
#include <vector>

#include "../Client/IO/IO.h"

struct WindowProperties {
	int WindowSizeX = 1280;
	int WindowSizeY = 720;

	bool WindowSizeDirty = false;
	bool Initialized = false;

	bool DrawSolid = true;

};

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
	
	void UpdateWindowName(std::string name);

	void renderSolid();
	void renderLines();

	void MousePositionCallback(double xpos, double ypos);
	void MouseButtonCallback(int button, int action);
	void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void ResizeWindowCallback(int x, int y);
	void ScrollCallback(GLFWwindow* win, double xoffset, double yoffset);

	UserInputs Inputs;

	WindowProperties Properties;

private:
	static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);

	
	GLFWwindow* window = nullptr;
};
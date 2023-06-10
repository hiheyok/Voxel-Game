#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include "../utils/LogUtils.h"
#include <vector>
#define _CRTDBG_MAP_ALLOC

class Window {
public:
	void Start();

	int cursorx = 0;
	int cursory = 0;

	void Refresh();
	void PollInputs();
	GLFWwindow* getWindow();

protected:

	double cursormovementx = 0;
	double cursormovementy = 0;

	bool WindowCloseCheck();
	void DisableCursor();
	void EnableCursor();

	void Typing();

	bool TestForKeyInputs(int Key);

	void UpdateWindowName(std::string name);

	void renderSolid();
	void renderLines();

private:
	static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
	void resize_window(int x, int y);
	void mouse_callback(double xpos, double ypos);
	int sizex = 1280;
	int sizey = 720;

	int cursorlastx = 0;
	int cursorlasty = 0;

	LogUtils* Logger  = nullptr;
	GLFWwindow* window = nullptr;
	bool init = false;
};
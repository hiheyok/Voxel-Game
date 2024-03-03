#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include "../Utils/LogUtils.h"
#include <unordered_set>
#include <vector>

#define KEY_CTRL 0x01

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
	void onMouseButton(int button, int action);
	void Typing();

	bool TestForKeyInputs(int Key);

	void UpdateWindowName(std::string name);

	void renderSolid();
	void renderLines();
	void mouse_callback(double xpos, double ypos);

	std::unordered_set<char> GetKeyboardDump();

	bool PressedRight = false;
	bool PressedLeft = false;
	bool PressedMiddle = false;

	void UpdateKeyPressSet();

	int sizex = 1280;
	int sizey = 720;

	bool WindowSizeDirty = false;
	bool DrawSolid = true;

private:
	static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
	void resize_window(int x, int y);
	
	

	int cursorlastx = 0;
	int cursorlasty = 0;

	bool firstMouse = true;

	std::unordered_set<char> KeyPressed;

	GLFWwindow* window = nullptr;
	bool init = false;
};
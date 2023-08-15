#pragma once
#include "Vulkan/Instance.h"

class Window {
public:
	int Initialize(const char* name = "Default", uint32_t width = 800, uint32_t height = 600);
	void mainLoop();
	void cleanup();
private:

	uint32_t m_width;
	uint32_t m_height;

	GLFWwindow* m_window;
	GraphicsInstance m_vkinstance;
};
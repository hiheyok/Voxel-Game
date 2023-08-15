#include "Window.h"
#include <exception>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

int Window::Initialize(const char* name, uint32_t width, uint32_t height) {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(width, height, name, nullptr, nullptr);
	m_width = width;
	m_height = height;

	m_vkinstance.Initialize(name, "No Engine", VK_API_VERSION_1_3);

	return -1;
}

void Window::cleanup() {
	m_vkinstance.cleanup();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}
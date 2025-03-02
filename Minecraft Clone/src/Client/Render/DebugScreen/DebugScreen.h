#pragma once
#include "../../../RenderEngine/GUI/TextRenderer.h"
#include <GLFW/glfw3.h>

class DebugScreen {
private:
	GLFWwindow* window = nullptr;
	TextRenderer renderer;
	float fps;
	int VRAMUsageMB = 0;
	glm::vec3 position_ = glm::vec3(0.f, 0.f, 0.f);

public:
	void Render();

	void Update();

	void EditText(std::string name, const char* c);

	void EditText(std::string name, std::string text);

	void Initialize(GLFWwindow* w);
};
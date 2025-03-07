#pragma once
#include "../../../RenderEngine/GUI/TextRenderer.h"
#include <GLFW/glfw3.h>

class DebugScreen {
public:
    void Render();

    void Update();

    void EditText(std::string name, const char* c);

    void EditText(std::string name, std::string text);

    void Initialize(GLFWwindow* w);
private:
    GLFWwindow* window_{ nullptr };
    TextRenderer renderer_;
    float fps_{ 0.0f };
    int vram_usage_mb_{0};
    glm::vec3 position_{ 0.f, 0.f, 0.f };
};
#pragma once
#include <string>
#include <memory>
#include <glm/vec3.hpp>

class TextRenderer;

struct GLFWWindow;

class DebugScreen {
private:
    GLFWwindow* window_{ nullptr };
    std::unique_ptr<TextRenderer> renderer_;
    float fps_{ 0.0f };
    int vram_usage_mb_{ 0 };
    glm::vec3 position_{ 0.f, 0.f, 0.f };
public:
    DebugScreen();
    ~DebugScreen();
    void Render();
    void Update();
    void EditText(std::string name, const char* c);
    void EditText(std::string name, std::string text);
    void Initialize(GLFWwindow* w);
};
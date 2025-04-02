#pragma once
#include <vector>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "../Client/IO/IO.h"

struct WindowProperties {
    int window_size_x_ = 1280;
    int window_size_y_ = 720;

    bool window_size_dirty_ = false;
    bool initialized_ = false;
    bool draw_solid_ = true;
};

class Window {
public:
    void Start();
    void Refresh();
    void PollInputs();
    WindowProperties& GetProperties();
    UserInputs& GetUserInputs();
    GLFWwindow* GetWindow();

    void RenderSolid();
    void RenderLines();
protected:
    bool WindowCloseCheck();
    void DisableCursor();
    void EnableCursor();
    
    void UpdateWindowName(std::string name);

    void MousePositionCallback(double xpos, double ypos);
    void MouseButtonCallback(int button, int action);
    void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void ResizeWindowCallback(int x, int y);
    void ScrollCallback(GLFWwindow* win, double xoffset, double yoffset);

    UserInputs inputs_;
    WindowProperties properties_;
private:
    static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
    
    GLFWwindow* window_ = nullptr;
};
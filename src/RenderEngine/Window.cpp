#include "Window.h"

#include <iostream>
#include <sstream>
#include <string>

#define _CRTDBG_MAP_ALLOC

using namespace std;

void APIENTRY Window::glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam) {

    std::stringstream str;


    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    /* std::cout << "---------------" << std::endl;
     std::cout << "Debug message (" << id << "): " << message << std::endl;*/

     // std::cout  << "---------------" << std::endl;
    str << " (OPENGL) Debug message (" << id << "): " << message << " | ";



    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             str << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   str << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: str << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     str << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     str << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           str << "Source: Other"; break;
    } str << " | ";

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               str << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: str << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  str << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         str << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         str << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              str << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          str << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           str << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               str << "Type: Other"; break;
    } str << " | ";

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         str << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       str << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          str << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: str << "Severity: notification"; break;
    }

    str.seekg(0, ios::end);


    if (str.str().size() != 0) {
        getLogger()->LogError("OpenGL", str.str());
    }

}

GLFWwindow* Window::getWindow() {
    return window;
}

bool Window::WindowCloseCheck() {
    return glfwWindowShouldClose(window);
}

void Window::Start() {

    if (init) {
        getLogger()->LogError("OpenGL","Already initialized");
        return;
    }

    Logger = getLogger();
    glfwInit();
    if (!glfwInit())
    {
        Logger->LogError("OpenGL", "Initialization Failed: GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(sizex, sizey, "1.1.0A (DEV)", NULL, NULL);

    glfwMakeContextCurrent(window);

    if (window == nullptr)
    {
        Logger->LogError("OpenGL", "Failed to create GLFW Window");

        glfwTerminate();
        return;
    }
    else {
        Logger->LogInfo("OpenGL","Created GLFW Window");
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int a, int b) { static_cast<Window*>(glfwGetWindowUserPointer(win))->resize_window(a, b); });
    glfwSetCursorPosCallback(window, +[](GLFWwindow* win, double a, double b) { static_cast<Window*>(glfwGetWindowUserPointer(win))->mouse_callback(a, b); });
    // glfwSetScrollCallback(window, +[](GLFWwindow* win, double a, double b) { static_cast<Window*>(glfwGetWindowUserPointer(win))->scroll_callback(win, a, b); });
    glewExperimental = GL_TRUE;
    glewInit();

    if (glewInit() != GLEW_OK) {
        Logger->LogError("OpenGL", "Initialization Failed: GLEW");
        return;
    }
    std::stringstream str;
    str << "OpenGL Version: " << glGetString(GL_VERSION);
    Logger->LogInfo("OpenGL", str.str());

    

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    
    glfwSwapInterval(0);

 
}

void Window::mouse_callback(double xpos, double ypos) {

    cursormovementx = xpos - cursorx;
    cursormovementy = ypos - cursory;

    cursorx = (int)xpos;
    cursory = (int)ypos;

}

void Window::UpdateWindowName(std::string name) {
    glfwSetWindowTitle(window, name.c_str());
}

void Window::renderLines() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


void Window::renderSolid() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Window::resize_window(int x, int y) {
    glViewport(0, 0, x, y);
    sizex = x;
    sizey = y;
    std::stringstream str;
    Logger->LogInfo("OpenGL"," Resized Window: " + std::to_string(sizex) + ", " + std::to_string(sizey));
}

void Window::Refresh() {
    glfwSwapBuffers(window);
}

void Window::PollInputs() {
    glfwPollEvents();
}

void Window::DisableCursor() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::EnableCursor() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool Window::TestForKeyInputs(int Key) {
    if (glfwGetKey(window,Key) == GLFW_PRESS)
        return true;
    return false;
}
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
        Logger.LogError("OpenGL", str.str());
        std::cout << "OpenGL: " << str.str() << "\n";
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
        Logger.LogError("OpenGL","Already initialized");
        return;
    }

    glfwInit();
    if (!glfwInit())
    {
        Logger.LogError("OpenGL", "Initialization Failed: GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(sizex, sizey, "1.1.1A (DEV)", NULL, NULL);

    glfwMakeContextCurrent(window);

    if (window == nullptr)
    {
        Logger.LogError("OpenGL", "Failed to create GLFW Window");

        glfwTerminate();
        return;
    }
    else {
        Logger.LogInfo("OpenGL","Created GLFW Window");
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int a, int b) { static_cast<Window*>(glfwGetWindowUserPointer(win))->resize_window(a, b); });
    glfwSetCursorPosCallback(window, +[](GLFWwindow* win, double a, double b) { static_cast<Window*>(glfwGetWindowUserPointer(win))->mouse_callback(a, b); });
    glfwSetMouseButtonCallback(window, +[](GLFWwindow* win, int a, int b, int c) { static_cast<Window*>(glfwGetWindowUserPointer(win))->onMouseButton(a, b); });
    
    // glfwSetScrollCallback(window, +[](GLFWwindow* win, double a, double b) { static_cast<Window*>(glfwGetWindowUserPointer(win))->scroll_callback(win, a, b); });
    glewExperimental = GL_TRUE;
    glewInit();

    if (glewInit() != GLEW_OK) {
        Logger.LogError("OpenGL", "Initialization Failed: GLEW");
        return;
    }

    std::stringstream str;
    str << "OpenGL Version: " << glGetString(GL_VERSION);
    Logger.LogInfo("OpenGL", str.str());

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
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
    Logger.LogInfo("OpenGL"," Resized Window: " + std::to_string(sizex) + ", " + std::to_string(sizey));
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

void Window::UpdateKeyPressSet() {
    KeyPressed.clear();

    if (TestForKeyInputs(GLFW_KEY_0))
        KeyPressed.insert('0');
    if (TestForKeyInputs(GLFW_KEY_1))
        KeyPressed.insert('1');
    if (TestForKeyInputs(GLFW_KEY_2))
        KeyPressed.insert('2');
    if (TestForKeyInputs(GLFW_KEY_3))
        KeyPressed.insert('3');
    if (TestForKeyInputs(GLFW_KEY_4))
        KeyPressed.insert('4');
    if (TestForKeyInputs(GLFW_KEY_5))
        KeyPressed.insert('5');
    if (TestForKeyInputs(GLFW_KEY_6))
        KeyPressed.insert('6');
    if (TestForKeyInputs(GLFW_KEY_7))
        KeyPressed.insert('7');
    if (TestForKeyInputs(GLFW_KEY_8))
        KeyPressed.insert('8');
    if (TestForKeyInputs(GLFW_KEY_9))
        KeyPressed.insert('9');

    if (TestForKeyInputs(GLFW_KEY_Q))
        KeyPressed.insert('Q');
    if (TestForKeyInputs(GLFW_KEY_W))
        KeyPressed.insert('W');
    if (TestForKeyInputs(GLFW_KEY_E))
        KeyPressed.insert('E');
    if (TestForKeyInputs(GLFW_KEY_R))
        KeyPressed.insert('R');
    if (TestForKeyInputs(GLFW_KEY_T))
        KeyPressed.insert('T');
    if (TestForKeyInputs(GLFW_KEY_Y))
        KeyPressed.insert('Y');
    if (TestForKeyInputs(GLFW_KEY_U))
        KeyPressed.insert('U');
    if (TestForKeyInputs(GLFW_KEY_I))
        KeyPressed.insert('I');
    if (TestForKeyInputs(GLFW_KEY_O))
        KeyPressed.insert('O');
    if (TestForKeyInputs(GLFW_KEY_P))
        KeyPressed.insert('P');

    if (TestForKeyInputs(GLFW_KEY_A))
        KeyPressed.insert('A');
    if (TestForKeyInputs(GLFW_KEY_S))
        KeyPressed.insert('S');
    if (TestForKeyInputs(GLFW_KEY_D))
        KeyPressed.insert('D');
    if (TestForKeyInputs(GLFW_KEY_F))
        KeyPressed.insert('F');
    if (TestForKeyInputs(GLFW_KEY_G))
        KeyPressed.insert('G');
    if (TestForKeyInputs(GLFW_KEY_H))
        KeyPressed.insert('H');
    if (TestForKeyInputs(GLFW_KEY_J))
        KeyPressed.insert('J');
    if (TestForKeyInputs(GLFW_KEY_K))
        KeyPressed.insert('K');
    if (TestForKeyInputs(GLFW_KEY_L))
        KeyPressed.insert('L');

    if (TestForKeyInputs(GLFW_KEY_Z))
        KeyPressed.insert('Z');
    if (TestForKeyInputs(GLFW_KEY_X))
        KeyPressed.insert('X');
    if (TestForKeyInputs(GLFW_KEY_C))
        KeyPressed.insert('C');
    if (TestForKeyInputs(GLFW_KEY_V))
        KeyPressed.insert('V');
    if (TestForKeyInputs(GLFW_KEY_B))
        KeyPressed.insert('B');
    if (TestForKeyInputs(GLFW_KEY_N))
        KeyPressed.insert('N');
    if (TestForKeyInputs(GLFW_KEY_M))
        KeyPressed.insert('M');

    if (TestForKeyInputs(GLFW_KEY_SPACE))
        KeyPressed.insert(' ');

    if (TestForKeyInputs(GLFW_KEY_LEFT_CONTROL))
        KeyPressed.insert(KEY_CTRL);


}

void Window::onMouseButton(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        PressedRight = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        PressedLeft = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        PressedMiddle = true;
    }
}

std::unordered_set<char> Window::GetKeyboardDump() {
    return KeyPressed;
}
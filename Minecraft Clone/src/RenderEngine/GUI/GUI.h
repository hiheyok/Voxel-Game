#pragma once
#include <memory>
#include <vector>
#include <string>

class GUISet;
class Shader;

struct GLFWwindow;

class GUI {
public:
    GUI();
    ~GUI();

    void Initialize(GLFWwindow* win);
    size_t AddGUI(std::string Name, GUISet set);
    GUISet& EditGUISet(size_t GUIIndex);
    void PrepareRenderer();
    void Render();

private:
    void SetupDrawCalls();
    void Update();

    std::vector<GUISet> guis_;
    std::unique_ptr<Shader> shader_;
    GLFWwindow* window_ = nullptr;
};
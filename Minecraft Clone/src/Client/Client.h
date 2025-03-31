#pragma once
#include "../RenderEngine/Window.h"

class MainPlayer;
class InternalServer;
class WorldRender;
class MultiEntityRenderer;
class EntityRendererUpdater;
class TexturedFrameBuffer;
class TextRenderer;
class DebugScreen;
class PerformanceProfiler;

class Client : protected Window {
public:
    Client();
    ~Client();

    void run();
private:
    void Initialize();
    void GameLoop();
    void Update();
    void Cleanup();
    void Render();
    void SetDebugScreen();

    double frametime_ = 0.5;

    std::unique_ptr<MainPlayer> main_player_;

    std::unique_ptr<InternalServer> server_ = nullptr;
    std::unique_ptr<WorldRender> terrain_render_ = nullptr;
    std::unique_ptr<MultiEntityRenderer> entity_render_ = nullptr;
    std::unique_ptr<EntityRendererUpdater> entity_updater_ = nullptr;
    std::unique_ptr<TexturedFrameBuffer> framebuffer_ = nullptr;
    std::unique_ptr<TextRenderer> text_render_ = nullptr;
    std::unique_ptr<DebugScreen> debug_screen_ = nullptr;
    PerformanceProfiler* profiler_ = nullptr;
};
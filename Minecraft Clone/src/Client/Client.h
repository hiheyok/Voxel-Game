#pragma once
#include "../RenderEngine/Window.h"

class MainPlayer;
class WorldRender;
class MultiEntityRenderer;
class EntityRendererUpdater;
class TexturedFrameBuffer;
class TextRenderer;
class DebugScreen;
class PerformanceProfiler;
class InternalInterface;
class ClientLevel;
class Server;

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
    void InitializeServerCom();
    void InitializeGameContent();

    double frametime_ = 0.5;

    EntityUUID player_uuid_;
    std::unique_ptr<MainPlayer> main_player_;
    std::unique_ptr<Server> server_;
    std::unique_ptr<ClientLevel> client_level_;
    std::unique_ptr<InternalInterface> internal_interface_;
    std::unique_ptr<WorldRender> terrain_render_;
    std::unique_ptr<MultiEntityRenderer> entity_render_;
    std::unique_ptr<EntityRendererUpdater> entity_updater_;
    std::unique_ptr<TexturedFrameBuffer> framebuffer_;
    std::unique_ptr<TextRenderer> text_render_;
    std::unique_ptr<DebugScreen> debug_screen_;
    PerformanceProfiler* profiler_;
};
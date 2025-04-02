#include <chrono>
#include "Client/Client.h"
#include "Client/ClientPlay.h"
#include "Player/MainPlayer.h"
#include "Profiler/PerformanceProfiler.h"
#include "Render/DebugScreen/DebugScreen.h"
#include "Render/WorldRender.h"
#include "Core/Options/Option.h"
#include "Level/Chunk/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Item/Items.h"
#include "Level/Item/ItemTextureAtlas.h"
#include "Level/Entity/Entities.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Timer/Timer.h"
#include "Level/Server/Server.h"
#include "Level/Server/Interfaces/InternalInterface.h"
#include "Level/Server/Networking/PlayerAction.h"
#include "Level/Server/Networking/ChunkUpdate.h"
#include "Level/Server/Networking/Packet.h"
#include "Level/LevelLoader.h"
#include "Level/Level.h"
#include "RenderEngine/ChunkRenderer/TerrainRenderer.h"
#include "RenderEngine/EntityRenderer/MultiEntityRender.h"
#include "RenderEngine/EntityRenderer/EntityRenderUpdate.h"
#include "RenderEngine/GUI/TextRenderer.h"
#include "RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "Utils/LogUtils.h"

Client::Client() : 
    server_{ std::make_unique<Server>() },
    entity_render_{ std::make_unique<MultiEntityRenderer>() },
    entity_updater_{ std::make_unique<EntityRendererUpdater>() },
    text_render_{ std::make_unique<TextRenderer>() },
    debug_screen_{ std::make_unique<DebugScreen>() },
    internal_interface_{ std::make_unique<InternalInterface>() },
    
    profiler_{ new PerformanceProfiler()} { }

Client::~Client() = default;

void Client::InitializeServerCom() {
    // Start Server First
    ServerSettings settings_;
    settings_.gen_thread_count_ = g_app_options.world_gen_threads_;
    settings_.light_engine_thread_count_ = g_app_options.light_engine_threads_;
    settings_.horizontal_ticking_distance_ = g_app_options.horizontal_render_distance_;
    settings_.vertical_ticking_distance_ = g_app_options.vertical_render_distance_;

    // Joins the server it should start receiving stuff now
    server_->StartServer(settings_);
    player_uuid_ = server_->SetInternalConnection(internal_interface_.get());
    client_play_ = std::make_unique<ClientPlay>(internal_interface_.get(), this, profiler_);

}
void Client::InitializeGameContent() {
    g_blocks.Initialize();
    g_entity_list.Initialize();
    //entity_render_->Initialize(profiler_);
    //entity_render_->SetWindow(GetWindow());
    text_render_->InitializeTextRenderer(GetWindow());
    g_items.RegisterAll();
    g_item_atlas.Initialize(512 * 16 * 2, 16 * 2 * 8);
    for (auto& item : g_items.item_container_) {
        g_item_atlas.AddItem(item.second);
    }

}

void Client::Initialize() {
    Start();
    DisableCursor();
    InitializeGameContent();


    InitializeServerCom();

    //entity_updater_->SetEntityRenderer(entity_render_.get(), server_->GetTickClock());
    //entity_updater_->Start(server_->server->level_->main_world_.get());


    debug_screen_->Initialize(GetWindow());
}

void Client::run() {
    Initialize();
    GameLoop();
    Cleanup();
}

void Client::Cleanup() {
    //entity_render_->Clean();
    server_->Stop();
    g_logger.Stop();
    //entity_updater_->Stop();
    g_blocks.CleanUp();
    glfwDestroyWindow(GetWindow());
}

void Client::SetDebugScreen() {
    debug_screen_->EditText("Stat5", "FPS: " + std::to_string(1.0 / frametime_));
    debug_screen_->EditText("Stat8", "Event Queue Size: N/A");
    debug_screen_->EditText("Stat9", "Server Tick (MSPT): " + std::to_string(server_->GetMSPT()));
    debug_screen_->EditText("Stat10","Chunk Count: " + std::to_string(server_->GetChunkCount()));
    debug_screen_->Update();
}

void Client::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    client_play_->Render(this);
    debug_screen_->Render();
}

void Client::GameLoop() {

    Timer time;

    while (!WindowCloseCheck()) {
        Timer FrametimeTracker;

        Update();

        Render();
        profiler_->ProfileStart("root/refresh");
        Refresh();
        profiler_->ProfileStop("root/refresh");
        frametime_ = FrametimeTracker.GetTimePassed_s();

        inputs_.delta_ = frametime_;

        if (time.GetTimePassed_ms() > 50) {
            SetDebugScreen();
            time.Set();
        }
        
    }
}

void Client::Update() {
    PollInputs();

    if (inputs_.CheckKey(GLFW_KEY_F)) {
        properties_.draw_solid_ = false;
    }
    if (inputs_.CheckKey(GLFW_KEY_G)) {
        properties_.draw_solid_ = true;
    }

    if (inputs_.CheckKeyPress(GLFW_KEY_R)) {
        entity_render_->Reload();
    }

    if (inputs_.CheckKeyPress(GLFW_KEY_P)) {
        profiler_->LoadCache();
        profiler_->print();
    }

    if (inputs_.CheckKey(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(GetWindow(), true);
    }
    
    client_play_->Update(this);

    inputs_.UpdateAllKey();
}

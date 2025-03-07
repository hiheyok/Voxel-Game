#include "Client.h"
#include "../Utils/LogUtils.h"
#include "../Level/Chunk/Block/Blocks.h"
#include "../Level/Entity/Entities.h"
#include <chrono>
#include "../Level/Timer/Timer.h"
#include "../Core/Options/Option.h"

using namespace std;
using namespace chrono;
using namespace glm;

void Client::Initialize() {
    Start();

    g_blocks.Initialize();
    g_entity_list.Initialize();
    entity_render_.Initialize(&profiler_);
    entity_render_.SetWindow(GetWindow());
    text_render_.InitializeTextRenderer(GetWindow());
    g_items.RegisterAll();
    g_item_atlas.Initialize(512*16 * 2, 16*2*8);

    for (auto& item : g_items.item_container_) {
        g_item_atlas.AddItem(item.second);
    }

    framebuffer_.GenBuffer(properties_.window_size_x_, properties_.window_size_y_, (float)g_app_options.graphics_scale_, GL_RGB);

    DisableCursor();

    server_.startInternalServer(&main_player_.player_);

    main_player_.Initialize(GetWindow(), &server_);
    main_player_.SetPlayerPosition(0., 50, 0.);
    main_player_.SetPlayerRotation(-135.f, -30.);

    entity_updater_.SetEntityRenderer(&entity_render_, server_.GetTickClock());

    entity_updater_.Start();

    g_logger.LogInfo("World", "Generating World");
    terrain_render_.Start(GetWindow(), &server_, &profiler_);
    g_logger.LogInfo("Client", "Starting Gameloop");

    debug_screen_.Initialize(GetWindow());
}

void Client::run() {
    Initialize();
    GameLoop();
    Cleanup();
}

void Client::Cleanup() {
    entity_render_.Clean();
    terrain_render_.Stop();
    server_.Stop();
    g_logger.Stop();
    entity_updater_.Stop();
    g_blocks.CleanUp();
    glfwDestroyWindow(GetWindow());
}

void Client::SetDebugScreen() {
    debug_screen_.EditText("Stat1", "VRAM Usage: " + to_string((double)terrain_render_.renderer_v2_.getVRAMUsageFull() / 1000000.0) + " MB");
    debug_screen_.EditText("Stat2", "XYZ: " + to_string(main_player_.GetEntityProperties().position_.x) + "/" + to_string(main_player_.GetEntityProperties().position_.y) + "/" + std::to_string(main_player_.GetEntityProperties().position_.z));
    debug_screen_.EditText("Stat3", "Velocity XYZ: " + to_string(main_player_.GetEntityProperties().velocity_.x) + "/" + to_string(main_player_.GetEntityProperties().velocity_.y) + "/" + std::to_string(main_player_.GetEntityProperties().velocity_.z));
    debug_screen_.EditText("Stat4", "VRAM Fragmentation Rate: " + to_string(terrain_render_.renderer_v2_.getFragmentationRate() * 100) + "%");
    debug_screen_.EditText("Stat5", "FPS: " + to_string(1.0 / frametime_));
    debug_screen_.EditText("Stat6", "Mesh Stats (ms) Total/S0/S1/S2: " + to_string(terrain_render_.build_time_ / 1000.f) + "/" + to_string(terrain_render_.build_stage_0_ / 1000.f) + "/" + std::to_string(terrain_render_.build_stage_1_ / 1000.f) + "/" + std::to_string(terrain_render_.build_stage_2_ / 1000.f));
    debug_screen_.EditText("Stat7", "Lighting Engine Queued: " + to_string(server_.server->level_.level_loader_.GetLightEngineQueueSize()));
    debug_screen_.EditText("Stat8", "Event Queue Size: N/A");
    debug_screen_.EditText("Stat9", "Server Tick (MSPT): " + to_string(server_.GetMSPT()));
    debug_screen_.EditText("Stat10","Chunk Count: " + std::to_string(server_.GetChunkCount()));
    debug_screen_.Update();
}

void Client::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    framebuffer_.BindFBO();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!properties_.draw_solid_)
        RenderLines();

    entity_render_.Render();
    terrain_render_.Render();

    if (!properties_.draw_solid_)
        RenderSolid();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    framebuffer_.UnbindFBO();

    framebuffer_.Render();

    main_player_.RenderGUIs();
    debug_screen_.Render();
}

void Client::GameLoop() {

    Timer time;

    while (!WindowCloseCheck()) {
        Timer FrametimeTracker;

        Update();

        Render();
        profiler_.ProfileStart("root/refresh");
        Refresh();
        profiler_.ProfileStop("root/refresh");
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
        entity_render_.Reload();
    }

    if (inputs_.CheckKeyPress(GLFW_KEY_P)) {
        profiler_.LoadCache();
        profiler_.print();
    }

    if (inputs_.CheckKey(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(GetWindow(), true);
    }



    if (properties_.window_size_dirty_) {
        properties_.window_size_dirty_ = false;

        framebuffer_.Clear();
        framebuffer_.GenBuffer(properties_.window_size_x_, properties_.window_size_y_, (float)g_app_options.graphics_scale_);
    }

    main_player_.Update(inputs_);

    terrain_render_.SetPosition(main_player_.GetEntityProperties().position_);
    terrain_render_.SetRotation(main_player_.GetEntityProperties().rotation_);

    entity_render_.SetPosition(main_player_.GetEntityProperties().position_);
    entity_render_.SetRotation(main_player_.GetEntityProperties().rotation_);

    entity_render_.SetTimePastTick(server_.GetTickClock()->GetTimePassed_s());
    entity_render_.Update();

    terrain_render_.Update();

    inputs_.UpdateAllKey();
}

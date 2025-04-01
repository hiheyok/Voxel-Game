#include <chrono>
#include "Client.h"
#include "ClientLevel/ClientCache.h"
#include "ClientLevel/ClientLevel.h"
#include "Player/MainPlayer.h"
#include "Profiler/PerformanceProfiler.h"
#include "Render/DebugScreen/DebugScreen.h"
#include "Render/WorldRender.h"
#include "../Core/Options/Option.h"
#include "../Level/Chunk/Block/Blocks.h"
#include "../Level/Chunk/Chunk.h"
#include "../Level/Item/Items.h"
#include "../Level/Item/ItemTextureAtlas.h"
#include "../Level/Entity/Entities.h"
#include "../Level/Entity/Mobs/Player.h"
#include "../Level/Timer/Timer.h"
#include "../Level/Server/Server.h"
#include "../Level/Server/Interfaces/InternalInterface.h"
#include "../Level/Server/Networking/PlayerAction.h"
#include "../Level/Server/Networking/ChunkUpdate.h"
#include "../Level/Server/Networking/Packet.h"
#include "../Level/LevelLoader.h"
#include "../Level/Level.h"
#include "../RenderEngine/ChunkRenderer/TerrainRenderer.h"
#include "../RenderEngine/EntityRenderer/MultiEntityRender.h"
#include "../RenderEngine/EntityRenderer/EntityRenderUpdate.h"
#include "../RenderEngine/GUI/TextRenderer.h"
#include "../RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "../Utils/LogUtils.h"

Client::Client() : 
    server_{ std::make_unique<Server>() },
    terrain_render_{ std::make_unique<WorldRender>() },
    entity_render_{ std::make_unique<MultiEntityRenderer>() },
    entity_updater_{ std::make_unique<EntityRendererUpdater>() },
    framebuffer_{ std::make_unique<TexturedFrameBuffer>() },
    text_render_{ std::make_unique<TextRenderer>() },
    debug_screen_{ std::make_unique<DebugScreen>() },
    main_player_{ std::make_unique<MainPlayer>() },
    internal_interface_{ std::make_unique<InternalInterface>() },
    client_level_{ std::make_unique<ClientLevel>() },
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
    main_player_->player_->properties_.entity_uuid_ = player_uuid_;
    main_player_->Initialize(GetWindow(), internal_interface_.get(), &client_level_->cache);
    main_player_->SetPlayerPosition(0., 50, 0.);
    main_player_->SetPlayerRotation(-135.f, -30.);

    // Send server update on player position
    PlayerPacket::PlayerMove packet;
    packet.acc_ = main_player_->player_->properties_.acceleration_;
    packet.vel_ = main_player_->player_->properties_.velocity_;
    packet.pos_ = main_player_->player_->properties_.position_;
    internal_interface_->SendPlayerAction(packet);
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

    framebuffer_->GenBuffer(properties_.window_size_x_, properties_.window_size_y_, (float)g_app_options.graphics_scale_, GL_RGB);
}

void Client::Initialize() {
    Start();
    DisableCursor();
    InitializeGameContent();
    InitializeServerCom();
    

    

    

    //entity_updater_->SetEntityRenderer(entity_render_.get(), server_->GetTickClock());
    //entity_updater_->Start(server_->server->level_->main_world_.get());

    g_logger.LogInfo("Client::Initialize", "Generating World");
    terrain_render_->Start(GetWindow(), &client_level_->cache, profiler_);
    g_logger.LogInfo("Client::Initialize", "Starting Gameloop");

    debug_screen_->Initialize(GetWindow());
}

void Client::run() {
    Initialize();
    GameLoop();
    Cleanup();
}

void Client::Cleanup() {
    //entity_render_->Clean();
    terrain_render_->Stop();
    server_->Stop();
    g_logger.Stop();
    //entity_updater_->Stop();
    g_blocks.CleanUp();
    glfwDestroyWindow(GetWindow());
}

void Client::SetDebugScreen() {
    debug_screen_->EditText("Stat1", "VRAM Usage: " + std::to_string((double)terrain_render_->renderer_->getVRAMUsageFull() / 1000000.0) + " MB");
    debug_screen_->EditText("Stat2", "XYZ: " + std::to_string(main_player_->GetEntityProperties().position_.x) + "/" + std::to_string(main_player_->GetEntityProperties().position_.y) + "/" + std::to_string(main_player_->GetEntityProperties().position_.z));
    debug_screen_->EditText("Stat3", "Velocity XYZ: " + std::to_string(main_player_->GetEntityProperties().velocity_.x) + "/" + std::to_string(main_player_->GetEntityProperties().velocity_.y) + "/" + std::to_string(main_player_->GetEntityProperties().velocity_.z));
    debug_screen_->EditText("Stat4", "VRAM Fragmentation Rate: " + std::to_string(terrain_render_->renderer_->getFragmentationRate() * 100) + "%");
    debug_screen_->EditText("Stat5", "FPS: " + std::to_string(1.0 / frametime_));
    debug_screen_->EditText("Stat6", "Mesh Stats (ms) Total/S0/S1/S2: " + std::to_string(terrain_render_->build_time_ / 1000.f) + "/" + std::to_string(terrain_render_->build_stage_0_ / 1000.f) + "/" + std::to_string(terrain_render_->build_stage_1_ / 1000.f) + "/" + std::to_string(terrain_render_->build_stage_2_ / 1000.f));
    debug_screen_->EditText("Stat7", "Mesh Engine Queued: " + std::to_string(terrain_render_->GetQueuedSize()));
    debug_screen_->EditText("Stat8", "Event Queue Size: N/A");
    debug_screen_->EditText("Stat9", "Server Tick (MSPT): " + std::to_string(server_->GetMSPT()));
    debug_screen_->EditText("Stat10","Chunk Count: " + std::to_string(server_->GetChunkCount()));
    debug_screen_->Update();
}

void Client::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    framebuffer_->BindFBO();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!properties_.draw_solid_)
        RenderLines();

    // entity_render_->Render();
    terrain_render_->Render();

    if (!properties_.draw_solid_)
        RenderSolid();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    framebuffer_->UnbindFBO();

    framebuffer_->Render();

    main_player_->RenderGUIs();
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



    if (properties_.window_size_dirty_) {
        properties_.window_size_dirty_ = false;

        framebuffer_->Clear();
        framebuffer_->GenBuffer(properties_.window_size_x_, properties_.window_size_y_, (float)g_app_options.graphics_scale_);
    }

    main_player_->Update(inputs_);

    terrain_render_->SetPosition(main_player_->GetEntityProperties().position_);
    terrain_render_->SetRotation(main_player_->GetEntityProperties().rotation_);

    /*entity_render_->SetPosition(main_player_->GetEntityProperties().position_);
    entity_render_->SetRotation(main_player_->GetEntityProperties().rotation_);

    entity_render_->SetTimePastTick(server_->GetTickClock()->GetTimePassed_s());
    entity_render_->Update();*/

    // Poll all of the chunk packets

    std::vector<Packet::ChunkUpdateData> packets;
    internal_interface_->PollChunkUpdates(packets);

    // Process Add chunk first then light packets

    std::vector<ChunkUpdatePacket::AddChunk> addChunkPackets;
    std::vector<ChunkUpdatePacket::LightUpdate> updateLightPackets;

    for (const auto& packet : packets) {
        switch (packet.type_) {
        case ChunkUpdatePacket::ADD_CHUNK:
            {
                const ChunkUpdatePacket::AddChunk& p = std::get<ChunkUpdatePacket::AddChunk>(packet.packet_);
                addChunkPackets.push_back(p);
            }
            break;
        case ChunkUpdatePacket::DELETE_CHUNK:
            // Doesn't exist right now
            // TODO: Implement later
            throw std::exception("dont call me");
            break;
        case ChunkUpdatePacket::LIGHT_UPDATE:
            {
                const ChunkUpdatePacket::LightUpdate& p = std::get<ChunkUpdatePacket::LightUpdate>(packet.packet_);
                updateLightPackets.push_back(p);
            }
            break;
        }
    }

    FastHashSet<ChunkPos> chunkToUpdateRender;

    for (const auto& chunk : addChunkPackets) {
        const ChunkRawData& data = chunk.chunk_;
        chunkToUpdateRender.insert(data.pos_);
        std::unique_ptr<Chunk> newChunk = std::make_unique<Chunk>(data);
        client_level_->cache.AddChunk(std::move(newChunk));
    }

    for (const auto& light : updateLightPackets) {
        const ChunkLightingContainer& l = light.light_;
        chunkToUpdateRender.insert(l.position_);
        if (client_level_->cache.GetChunk(l.position_) == nullptr) {
            continue;
        }

        client_level_->cache.GetChunk(l.position_)->lighting_->ReplaceData(l.getData());
    }

    std::vector<ChunkPos> toUpdate(chunkToUpdateRender.begin(), chunkToUpdateRender.end());
    terrain_render_->Update(toUpdate);

    // Send player pos update

    PlayerPacket::PlayerMove playerMove;
    playerMove.acc_ = main_player_->player_->properties_.acceleration_;
    playerMove.vel_ = main_player_->player_->properties_.velocity_;
    playerMove.pos_ = main_player_->player_->properties_.position_;

    internal_interface_->SendPlayerAction(playerMove);

    inputs_.UpdateAllKey();
}

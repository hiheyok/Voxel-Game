#include "Client/ClientPlay.h"
#include "ClientLevel/ClientCache.h"
#include "ClientLevel/ClientLevel.h"
#include "Client/Player/MainPlayer.h"
#include "Client/Profiler/PerformanceProfiler.h"
#include "Client/Render/DebugScreen/DebugScreen.h"
#include "Client/Render/WorldRender.h"
#include "Client/IO/IO.h"
#include "Client/Render/DebugScreen/DebugScreen.h"
#include "Core/Options/Option.h"
#include "Level/Chunk/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Entity/Entities.h"
#include "Level/Entity/Mobs/Player.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Networking/PlayerAction.h"
#include "Core/Networking/ChunkUpdate.h"
#include "Core/Networking/Packet.h"
#include "RenderEngine/ChunkRenderer/TerrainRenderer.h"
#include "RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "RenderEngine/Window.h"
#include "Utils/LogUtils.h"

ClientPlay::ClientPlay(ServerInterface* interface, Window* window, PerformanceProfiler* profiler) :
    interface_{interface},
    main_player_{ std::make_unique<MainPlayer>() },
    framebuffer_{ std::make_unique<TexturedFrameBuffer>() },
    client_level_{ std::make_unique<ClientLevel>() },
    debug_screen_{ std::make_unique<DebugScreen>() },
    terrain_render_{ std::make_unique<WorldRender>() } {

    main_player_->Initialize(window->GetWindow(), interface_, &client_level_->cache);
    main_player_->SetPlayerPosition(0., 50, 0.);
    main_player_->SetPlayerRotation(-135.f, -30.);

    debug_screen_->Initialize(window->GetWindow());

    // Send server update on player position
    PlayerPacket::PlayerMove packet;
    packet.acc_ = main_player_->player_->properties_.acceleration_;
    packet.vel_ = main_player_->player_->properties_.velocity_;
    packet.pos_ = main_player_->player_->properties_.position_;
    interface->SendPlayerAction(packet);
    terrain_render_->Start(window->GetWindow(), &client_level_->cache, profiler);
    framebuffer_->GenBuffer(window->GetProperties().window_size_x_, window->GetProperties().window_size_y_, (float)g_app_options.graphics_scale_, GL_RGB);
}
ClientPlay::~ClientPlay() {
    terrain_render_->Stop();
}

void ClientPlay::Render(Window* window) {
    framebuffer_->BindFBO();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!window->GetProperties().draw_solid_)
        window->RenderLines();

    // entity_render_->Render();
    terrain_render_->Render();

    if (!window->GetProperties().draw_solid_)
        window->RenderSolid();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    framebuffer_->UnbindFBO();
    framebuffer_->Render();

    main_player_->RenderGUIs();
    debug_screen_->Render();
}

void ClientPlay::Update(Window* window) {
    if (window->GetProperties().window_size_dirty_) {
        window->GetProperties().window_size_dirty_ = false;

        framebuffer_->Clear();
        framebuffer_->GenBuffer(window->GetProperties().window_size_x_, window->GetProperties().window_size_y_, (float)g_app_options.graphics_scale_);
    }


    main_player_->Update(window->GetUserInputs());

    terrain_render_->SetPosition(main_player_->GetEntityProperties().position_);
    terrain_render_->SetRotation(main_player_->GetEntityProperties().rotation_);

    /*entity_render_->SetPosition(main_player_->GetEntityProperties().position_);
    entity_render_->SetRotation(main_player_->GetEntityProperties().rotation_);

    entity_render_->SetTimePastTick(server_->GetTickClock()->GetTimePassed_s());
    entity_render_->Update();*/


    UpdateChunks();

    // Send player pos update

    PlayerPacket::PlayerMove playerMove;
    playerMove.acc_ = main_player_->player_->properties_.acceleration_;
    playerMove.vel_ = main_player_->player_->properties_.velocity_;
    playerMove.pos_ = main_player_->player_->properties_.position_;

    interface_->SendPlayerAction(playerMove);

    UpdateDebugStats();
}

void ClientPlay::UpdateDebugStats() {
    ServerStats stats = interface_->GetServerStats();

    debug_screen_->EditText("Stat1", "VRAM Usage: " + std::to_string((double)terrain_render_->renderer_->getVRAMUsageFull() / 1000000.0) + " MB");
    debug_screen_->EditText("Stat2", "XYZ: " + std::to_string(main_player_->GetEntityProperties().position_.x) + "/" + std::to_string(main_player_->GetEntityProperties().position_.y) + "/" + std::to_string(main_player_->GetEntityProperties().position_.z));
    debug_screen_->EditText("Stat3", "Velocity XYZ: " + std::to_string(main_player_->GetEntityProperties().velocity_.x) + "/" + std::to_string(main_player_->GetEntityProperties().velocity_.y) + "/" + std::to_string(main_player_->GetEntityProperties().velocity_.z));
    debug_screen_->EditText("Stat4", "VRAM Fragmentation Rate: " + std::to_string(terrain_render_->renderer_->getFragmentationRate() * 100) + "%");
    debug_screen_->EditText("Stat5", "FPS: " + std::to_string(1.0 / frametime_));
    debug_screen_->EditText("Stat6", "Mesh Stats (ms) Total/S0/S1/S2: " + std::to_string(terrain_render_->build_time_ / 1000.f) + "/" + std::to_string(terrain_render_->build_stage_0_ / 1000.f) + "/" + std::to_string(terrain_render_->build_stage_1_ / 1000.f) + "/" + std::to_string(terrain_render_->build_stage_2_ / 1000.f));
    debug_screen_->EditText("Stat7", "Mesh Engine Queued: " + std::to_string(terrain_render_->GetQueuedSize()));
    debug_screen_->EditText("Stat8", "Event Queue Size: N/A");
    debug_screen_->EditText("Stat9", "Server Tick (MSPT): " + std::to_string(stats.mspt_));
    debug_screen_->EditText("Stat10", "Chunk Count: " + std::to_string(stats.chunk_count_));
    debug_screen_->Update();
}

void ClientPlay::UpdateChunks() {
    std::vector<Packet::ChunkUpdateData> packets;
    interface_->PollChunkUpdates(packets);

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
            // TODO: Implement later
            g_logger.LogError("ClientPlay::UpdateChunks", "Don't call me");
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
        if (client_level_->cache.CheckChunk(data.pos_)) {
            ChunkContainer* c = client_level_->cache.GetChunk(data.pos_);
            c->SetData(data);
        }
        else {
            std::unique_ptr<Chunk> newChunk = std::make_unique<Chunk>(data);
            client_level_->cache.AddChunk(std::move(newChunk));
        }
    }


    for (const auto& light : updateLightPackets) {
        const LightStorage& l = light.light_;

        chunkToUpdateRender.insert(l.position_);
        client_level_->cache.GetChunk(l.position_)->lighting_->ReplaceData(l.GetData());
    }

    std::vector<ChunkPos> toUpdate(chunkToUpdateRender.begin(), chunkToUpdateRender.end());
    terrain_render_->Update(toUpdate);
}
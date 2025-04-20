#include "Client/ClientPlay.h"
#include "ClientLevel/ClientCache.h"
#include "ClientLevel/ClientLevel.h"
#include "Client/Player/MainPlayer.h"
#include "Client/Profiler/PerformanceProfiler.h"
#include "Client/Render/DebugScreen/DebugScreen.h"
#include "Client/Render/WorldRender.h"
#include "Client/IO/IO.h"
#include "Core/Options/Option.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Entity/Entities.h"
#include "Level/Entity/Mobs/Player.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Networking/PlayerAction.h"
#include "Core/Networking/ChunkUpdate.h"
#include "Core/Networking/Packet.h"
#include "RenderEngine/ChunkRender/TerrainRenderer.h"
#include "RenderEngine/EntityRender/MultiEntityRender.h"
#include "RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "RenderEngine/Window.h"
#include "Utils/LogUtils.h"

ClientPlay::ClientPlay(ServerInterface* interface, Window* window, PerformanceProfiler* profiler) :
    interface_{interface},
    framebuffer_{ std::make_unique<TexturedFrameBuffer>() },
    client_level_{ std::make_unique<ClientLevel>() },
    main_player_{ std::make_unique<MainPlayer>(window, interface, &client_level_->cache) },
    debug_screen_{ std::make_unique<DebugScreen>() },
    entity_render_{ std::make_unique<MultiEntityRender>(main_player_->GetPlayerPOV()) },
    terrain_render_{ std::make_unique<WorldRender>(main_player_->GetPlayerPOV()) } {

    main_player_->SetPlayerPosition(0., 50, 0.);
    main_player_->SetPlayerRotation(-135.f, -30.);

    debug_screen_->Initialize(window->GetWindow());
    debug_screen_->SetUpdateRate(200);
    entity_render_->Initialize(profiler);
    entity_render_->SetWindow(window->GetWindow());

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
    entity_render_->Render();

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

    if (window->GetUserInputs().CheckKeyPress(GLFW_KEY_R)) {
        entity_render_->Reload();
    }


    main_player_->Update(window->GetUserInputs());

    entity_render_->SetTimePastTick(interface_->time.GetTimePassed_s());
    entity_render_->Update();

    UpdateChunks();
    UpdateEntities();

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
    glm::vec3 playerPos = main_player_->GetEntityProperties().position_;
    BlockPos blockPos{ playerPos.x, playerPos.y, playerPos.z };
    BlockPos relBlockPos = blockPos& (kChunkDim - 1);
    int lightLvl = -1;
    if (client_level_->cache.CheckChunk(blockPos / kChunkDim)) {
        lightLvl = client_level_->cache.GetChunk(blockPos / kChunkDim)->lighting_->GetLighting(relBlockPos.x, relBlockPos.y, relBlockPos.z);
    }

    debug_screen_->EditText("Stat1", "VRAM Usage: " + std::to_string((double)terrain_render_->renderer_->GetVRAMUsageFull() / 1000000.0) + " MB");
    debug_screen_->EditText("Stat2", "XYZ: " + std::to_string(playerPos.x) + "/" + std::to_string(playerPos.y) + "/" + std::to_string(playerPos.z));
    debug_screen_->EditText("Stat3", "Velocity XYZ: " + std::to_string(main_player_->GetEntityProperties().velocity_.x) + "/" + std::to_string(main_player_->GetEntityProperties().velocity_.y) + "/" + std::to_string(main_player_->GetEntityProperties().velocity_.z));
    debug_screen_->EditText("Stat4", "VRAM Fragmentation Rate: " + std::to_string(terrain_render_->renderer_->GetFragmentationRate() * 100) + "%");
    debug_screen_->EditText("Stat5", "FPS: " + std::to_string(1.0 / frametime_));
    debug_screen_->EditText("Stat6", "Mesh Stats (ms) Total/S0/S1/S2: " + 
        std::to_string(terrain_render_->build_time_ / 1000.f) + "/" + 
        std::to_string(100.0 * terrain_render_->build_stage_0_ / terrain_render_->build_time_) + "/" +
        std::to_string(100.0 * terrain_render_->build_stage_1_ / terrain_render_->build_time_) + "/" + 
        std::to_string(terrain_render_->build_stage_2_ / 1000.f));
    
    
    debug_screen_->EditText("Stat7", "Mesh Engine Queued: " + std::to_string(terrain_render_->GetQueuedSize()));
    debug_screen_->EditText("Stat8", "Light Level: " + std::to_string(lightLvl));
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

void ClientPlay::UpdateEntities() {
    std::vector<EntityProperty> spawnedEntities;
    std::vector<EntityProperty> updatedEntities;
    std::vector<EntityUUID> despawnedEntities;

    std::vector<Packet::EntityUpdate> packets;

    interface_->PollEntityUpdates(packets);
    //g_logger.LogDebug("ClientPlay::UpdateEntities", std::to_string(packets.size()));

    for (const auto& packet : packets) {
        switch (packet.type_) {
        case EntityUpdatePacket::ENTITY_DESPAWN:
        {
            const EntityUpdatePacket::EntityDespawn& data = std::get<EntityUpdatePacket::EntityDespawn>(packet.packet_);
            despawnedEntities.push_back(data.uuid_);
            //g_logger.LogDebug("ClientPlay::UpdateEntities", "Updated entity");
            break;
        }
        case EntityUpdatePacket::ENTITY_INVENTORY_UPDATE:
        {
            const EntityUpdatePacket::EntityInventoryUpdate& data = std::get<EntityUpdatePacket::EntityInventoryUpdate>(packet.packet_);
            // N/A
            break;
        }
        case EntityUpdatePacket::ENTITY_MOVE:
        {
            const EntityUpdatePacket::EntityMove& data = std::get<EntityUpdatePacket::EntityMove>(packet.packet_);
            updatedEntities.push_back(data.properties_);
            //g_logger.LogDebug("ClientPlay::UpdateEntities", "Updated entity");
            break;
        }
        case EntityUpdatePacket::ENTITY_SPAWN:
        {
            const EntityUpdatePacket::EntitySpawn& data = std::get<EntityUpdatePacket::EntitySpawn>(packet.packet_);
            spawnedEntities.push_back(data.properties_);
            //g_logger.LogDebug("ClientPlay::UpdateEntities", "Updated entity");
            break;
        }
        default:
            g_logger.LogDebug("ClientPlay::UpdateEntities", "Default");

        }
    }

    for (const auto& entity : spawnedEntities) {
        client_level_->cache.InsertEntity(entity);
    }

    for (const auto& entity : updatedEntities) {
        client_level_->cache.UpdateEntity(entity);
    }

    for (const auto& entity : despawnedEntities) {
        client_level_->cache.RemoveEntity(entity);
    }

    // Insert it to the renderer now
    for (const auto& entity : client_level_->cache.entities_.GetChangedEntities()) {
        entity_render_->InsertEntity(entity);
    }

    for (const auto& entity : client_level_->cache.entities_.GetRemovedEntitiesUUID()) {
        entity_render_->RemoveEntity(entity);
    }
}
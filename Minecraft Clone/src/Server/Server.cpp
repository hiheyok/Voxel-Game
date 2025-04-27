#include "Server/Server.h"
#include "Core/Interfaces/ClientInterface.h"
#include "Core/Interfaces/InternalInterface.h"
#include "Level/Chunk/ChunkData.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Entity/Properties/EntityProperties.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Level.h"
#include "Level/World/WorldUpdater.h"
#include "Utils/Timer/Timer.h"
#include "Utils/Clock.h"
#include "Utils/Math/Ray/Ray.h"

Server::Server() = default;
Server::~Server() = default;

void Server::StartServer(ServerSettings serverSettings) {
    level_ = std::make_unique<Level>();
    level_->Start(static_cast<int>(serverSettings.gen_thread_count_), static_cast<int>(serverSettings.light_engine_thread_count_));
    time_ = std::make_unique<Timer>();
    stop_ = false;
    settings_ = std::make_unique<ServerSettings>(serverSettings);
    main_server_loop_ = std::thread(&Server::Loop, this);
}

void Server::Stop() {
    stop_ = true;
    main_server_loop_.join();
    level_->Stop();
}

void Server::Loop() {
    g_logger.LogDebug("Server::Loop", "Started main server loop");
    while (!stop_) {
        time_->Set();
        // Process Client -> Server events
        ProcessPacket();

        Tick();

        SendPacket();

        mspt_ = time_->GetTimePassed_ms();
        double time_left = (1.0 / (double)settings_->tick_rate_) - mspt_ / 1000.0;

        if (time_left > 0) {
            timerSleep(time_left);
        }

        mspt_ = time_->GetTimePassed_ms();
    }
    g_logger.LogDebug("Server::Loop", "Shutting down main server loop");
}

void Server::Tick() {
    level_->main_world_->Tick();
    level_->main_world_->EventTick();
    level_->updateDimensions();
    level_->main_world_->world_updater_->Load();
}

void Server::SendEvent(const Event& eventIn) {
    level_->main_world_->event_manager_.AddEvent(eventIn);
}

EntityUUID Server::SetInternalConnection(InternalInterface* interface) {
    client_interface_ = static_cast<ClientInterface*>(interface);
    std::unique_ptr<Player> player = std::make_unique<Player>();
    EntityUUID uuid = level_->main_world_->world_updater_->SetEntity(std::move(player));
    interface->SetPlayerUUID(uuid);
    return uuid;
}

void Server::ProcessPacket() {
    if (client_interface_ == nullptr) return;
    // Process player actions
    ProcessPlayerPackets(client_interface_);
}

void Server::ProcessPlayerPackets(ClientInterface* receiver) {
    std::vector<Packet::PlayerAction> playerPackets;
    receiver->PollClientPlayerAction(playerPackets);

    for (const auto& packet : playerPackets) {
        switch (packet.type_) {
        case PlayerPacket::GET_ITEM:
            // Send packet to player
            {
                const PlayerPacket::PlayerGetItem& getItem = std::get<PlayerPacket::PlayerGetItem>(packet.packet_);
                Entity* e = level_->main_world_->world_->GetEntity(receiver->GetPlayerUUID());
                Player* player = static_cast<Player*>(e);
                player->entity_inventory_.SetSlot(getItem.slot_, getItem.item_);

                EntityUpdatePacket::EntityInventoryUpdate inventoryUpdate;
                inventoryUpdate.item_ = getItem.item_;
                inventoryUpdate.slot_ = getItem.slot_;
                inventoryUpdate.uuid = receiver->GetPlayerUUID();
                receiver->SendEntityUpdate(inventoryUpdate);
        }
            break;
        case PlayerPacket::DESTROY_BLOCK:
            {
                const PlayerPacket::PlayerDestroyBlock& destroyBlock = std::get<PlayerPacket::PlayerDestroyBlock>(packet.packet_);
                BlockEvent blockEvent;
                blockEvent.block_ = g_blocks.AIR; 
                blockEvent.id_ = g_event_handler.BlockPlace;
                blockEvent.pos_ = destroyBlock.pos_;
                level_->main_world_->event_manager_.AddEvent(blockEvent);
            }
            break;
        case PlayerPacket::PLACE_BLOCK:
            {
                const PlayerPacket::PlayerPlaceBlock& placeBlock = std::get<PlayerPacket::PlayerPlaceBlock>(packet.packet_);
                BlockEvent blockEvent;
                blockEvent.block_ = placeBlock.block_;
                blockEvent.id_ = g_event_handler.BlockPlace;
                blockEvent.pos_ = placeBlock.pos_;
                level_->main_world_->event_manager_.AddEvent(blockEvent);
        }
            break;
        case PlayerPacket::MOVE:
            {
                const PlayerPacket::PlayerMove& movePlayer = std::get<PlayerPacket::PlayerMove>(packet.packet_);
                Entity* e = level_->main_world_->world_->GetEntity(receiver->GetPlayerUUID());
                e->properties_.acceleration_ = movePlayer.acc_;
                e->properties_.velocity_ = movePlayer.vel_;
                e->properties_.position_ = movePlayer.pos_;
        }
            break;
        }
    }
}

void Server::SendPacket() {
    if (client_interface_ == nullptr) return;
    SendEntityUpdatePacket(client_interface_);
    SendChunkUpdatePacket(client_interface_);
    SendServerStats(client_interface_);
    client_interface_->SendTimeLastTick();
}

void Server::SendEntityUpdatePacket(ClientInterface* receiver) {
    std::vector<EntityProperty> spawnedEntities = level_->main_world_->world_updater_->GetSpawnedEntities();
    std::vector<EntityProperty> entityUpdated = level_->main_world_->world_updater_->GetUpdatedEntities();
    std::vector<EntityUUID> entityDespawned = level_->main_world_->world_updater_->GetRemovedEntities();

    // Work on spawn entities
    for (const auto& entity : spawnedEntities) {
        EntityUpdatePacket::EntitySpawn packet;
        packet.properties_ = entity;
        receiver->SendEntityUpdate(packet);
    }

    // Work on updated entities
    for (const auto& entity : entityUpdated) {
        EntityUpdatePacket::EntityMove packet;
        packet.properties_ = entity;
        receiver->SendEntityUpdate(packet);
    }

    // Work on removed entities
    for (const auto& entity : entityDespawned) {
        EntityUpdatePacket::EntityDespawn packet;
        packet.uuid_ = entity;
        receiver->SendEntityUpdate(packet);
    }
}

void Server::SendChunkUpdatePacket(ClientInterface* receiver) {
    std::vector<ChunkPos> updatedChunks = level_->main_world_->world_updater_->GetUpdatedChunkPos();
    std::vector<ChunkPos> updatedLights = level_->main_world_->world_updater_->GetUpdatedLightPos();

    for (const auto& pos : updatedChunks) {
        ChunkContainer* chunk = level_->main_world_->world_->GetChunk(pos);
        if (chunk == nullptr) continue; // TODO: Fix this later
        ChunkRawData data = chunk->GetRawData();

        ChunkUpdatePacket::AddChunk packet;
        packet.chunk_ = data;
        receiver->SendChunkUpdates(packet);
    }

    for (const auto& pos : updatedLights) {
        ChunkContainer* chunk = level_->main_world_->world_->GetChunk(pos);
        LightStorage data = chunk->GetLightData();

        ChunkUpdatePacket::LightUpdate packet;
        packet.light_ = data;
        receiver->SendChunkUpdates(packet);
    }
}

void Server::SendServerStats(ClientInterface* receiver) {
    ServerStats stats;
    stats.chunk_count_ = 0;// level_->level_loader_->GetChunkCount(); TODO: Add chunk counter
    stats.mspt_ = mspt_;
    stats.event_queued_ = 0;
    receiver->SendServerStats(stats);
}
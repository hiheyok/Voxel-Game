#include "Server.h"
#include "Interfaces/ClientInterface.h"
#include "Interfaces/InternalInterface.h"
#include "Level/Chunk/ChunkPos/ChunkPos.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Entity/Properties/EntityProperties.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Level.h"
#include "Level/LevelLoader.h"
#include "Level/World/WorldInteraction/WorldLoader.h"
#include "Level/Timer/Timer.h"
#include "Utils/Clock.h"
#include "Utils/Math/Ray/Ray.h"

Server::Server() = default;
Server::~Server() = default;

size_t Server::GetChunkCount() {
    return level_->level_loader_->GetChunkCount();
}

double Server::GetMSPT() {
    return mspt_;
}

Timer* Server::GetTimer() {
    return time_.get();
}

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
        double timeLeft = (1000.0 / (double)settings_->tick_rate_) - mspt_;

        if (timeLeft > 0) {
            timerSleepNotPrecise(static_cast<int>(timeLeft));
        }
    }
    g_logger.LogDebug("Server::Loop", "Shutting down main server loop");
}

void Server::Tick() {
    level_->main_world_->Tick();
    level_->main_world_->EventTick();
    level_->updateDimensions();
    level_->main_world_->world_interactions_.worldLoader_->Load();
}

void Server::SendEvent(const Event& eventIn) {
    level_->main_world_->event_manager_.AddEvent(eventIn);
}

EntityUUID Server::SetInternalConnection(InternalInterface* interface) {
    client_interface_ = static_cast<ClientInterface*>(interface);
    std::unique_ptr<Player> player = std::make_unique<Player>();
    EntityUUID uuid = level_->main_world_->world_interactions_.AddEntity(std::move(player));
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
                Entity* e = level_->main_world_->world_interactions_.GetEntity(receiver->GetPlayerUUID());
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
                const PlayerPacket::PlayerDestroyBlock& destroyblock = std::get<PlayerPacket::PlayerDestroyBlock>(packet.packet_);
                level_->main_world_->world_interactions_.SetBlock(g_blocks.AIR, destroyblock.pos_);
            }
            break;
        case PlayerPacket::PLACE_BLOCK:
            {
                const PlayerPacket::PlayerPlaceBlock& placeBlock = std::get<PlayerPacket::PlayerPlaceBlock>(packet.packet_);
                level_->main_world_->world_interactions_.SetBlock(placeBlock.block_, placeBlock.pos_);
            }
            break;
        case PlayerPacket::MOVE:
            {
                const PlayerPacket::PlayerMove& movePlayer = std::get<PlayerPacket::PlayerMove>(packet.packet_);
                Entity* e = level_->main_world_->world_interactions_.GetEntity(receiver->GetPlayerUUID());
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
}

void Server::SendEntityUpdatePacket(ClientInterface* receiver) {
    std::vector<EntityProperty> spawnedEntities = level_->main_world_->world_interactions_.GetSpawnedEntities();
    std::vector<EntityProperty> entityUpdated = level_->main_world_->world_interactions_.GetUpdatedEntities();
    std::vector<EntityUUID> entityDespawned = level_->main_world_->world_interactions_.GetRemovedEntities();

    // Work on spawn entities
    for (const auto& entity : spawnedEntities) {
        EntityUpdatePacket::EntitySpawn packet;
        packet.properties_ = entity;
        receiver->SendEntityUpdate(packet);
    }

    // Work on updated entities
    for (const auto& entity : spawnedEntities) {
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
    std::vector<ChunkPos> updatedChunks = level_->main_world_->world_interactions_.GetUpdatedChunkPos();
    std::vector<ChunkPos> updatedLights = level_->main_world_->world_interactions_.GetUpdatedLightPos();

    for (const auto& pos : updatedChunks) {
        ChunkContainer* chunk = level_->main_world_->world_interactions_.GetChunk(pos);
        ChunkRawData data = chunk->GetRawData();

        ChunkUpdatePacket::AddChunk packet;
        packet.chunk_ = data;
        receiver->SendChunkUpdates(packet);
    }

    for (const auto& pos : updatedLights) {
        ChunkContainer* chunk = level_->main_world_->world_interactions_.GetChunk(pos);
        ChunkLightingContainer data = chunk->GetLightData();

        ChunkUpdatePacket::LightUpdate packet;
        packet.light_ = data;
        receiver->SendChunkUpdates(packet);
    }
}
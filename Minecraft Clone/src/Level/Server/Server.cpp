#include "Server.h"
#include "Interfaces/ClientInterface.h"
#include "Interfaces/InternalInterface.h"
#include "../Chunk/ChunkPos/ChunkPos.h"
#include "../Dimension/Dimension.h"
#include "../Entity/Properties/EntityProperties.h"
#include "../Entity/Mobs/Player.h"
#include "../Level.h"
#include "../LevelLoader.h"
#include "../World/WorldInteraction/WorldLoader.h"
#include "../Timer/Timer.h"
#include "../../Utils/Clock.h"
#include "../../Utils/Math/Ray/Ray.h"

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

std::vector<EntityProperty> Server::GetUpdatedEntities() {
    return level_->main_world_->world_interactions_.GetUpdatedEntities();
}

std::vector<EntityUUID> Server::GetRemovedEntities() {
    return level_->main_world_->world_interactions_.GetRemovedEntities();
}

bool Server::CheckEntityOnGround(EntityUUID id) {
    return level_->main_world_->world_interactions_.collusions_->isEntityOnGround(level_->main_world_->world_interactions_.GetEntity(id));
}

void Server::Join(Entity& entity) {
    level_->main_world_->world_interactions_.summonEntity(entity);
}

std::vector<ChunkPos> Server::GetUpdatedChunkPos() {
    return level_->main_world_->world_interactions_.GetUpdatedChunkPos();
}

Chunk* Server::GetChunk(const ChunkPos& pos) {
    return level_->main_world_->world_interactions_.GetChunk(pos);
}

BlockID Server::GetBlock(const BlockPos& pos) { //Include dimension in parameter later
    return level_->main_world_->world_interactions_.GetBlock(pos);
}

bool Server::GetRayIntersection(Ray& ray) { //Include dimension in paramter later
    return level_->main_world_->world_interactions_.collusions_->CheckRayIntersection(ray);
}

glm::vec3 Server::GetEntityCollusionTime(EntityUUID entity) {
    return level_->main_world_->world_interactions_.collusions_->GetTimeTillCollusion(level_->main_world_->world_interactions_.GetEntity(entity));
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

void Server::SendEvent(const Event& pEventIn) {
    level_->main_world_->event_manager_.AddEvent(pEventIn);
}

void Server::SetInternalConnection(InternalInterface* interface) {
    client_interface_ = static_cast<ClientInterface*>(interface);
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
        case PlayerPacket::PlACE_BLOCK:
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

// TODO: Add some caching stuff to allow for multiple players

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

    // Removed chunks later (unloading stuff)
}
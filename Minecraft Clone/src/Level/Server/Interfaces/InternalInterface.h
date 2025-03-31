#pragma once
#include "ServerInterface.h"
#include "../../../Utils/LogUtils.h"
#include "../../../Utils/Containers/ConcurrentQueue.h"
#include "ClientInterface.h"

//
class InternalInterface : public ServerInterface, public ClientInterface {
    bool is_connected_ = false;
public:
    InternalInterface() {

    }

    // Client -> Server
    void SendPlayerAction(const Packet::PlayerAction& action) override {
        player_action_queue_.Push(action);
    }

    void SendBlockAction(const Packet::BlockAction& action) override {
        block_action_queue_.Push(action);
    }

    bool IsConnected() {
        return is_connected_;
    }

    void Connect(const std::string& address, int port) override {
        g_logger.LogError("InternalServerInterface::Connect", "This function isn't active!");
    }

    void Disconnect() {

    }

    void Update() {

    }

    // Server -> Client

    void SendBlockUpdate(const Packet::BlockUpdate& update) {
        block_update_queue_.Push(update);
    }

    void SendEntitySpawns(const Packet::EntitySpawn& entitySpawn) {
        entity_spawn_queue_.Push(entitySpawn);
    }

    void SendEntityUpdate(const Packet::EntityUpdate& update) {
        entity_update_queue_.Push(update);
    }

    void SendEntityDespawns(const Packet::EntityDespawn& entityDespawn) {
        entity_despawn_queue_.Push(entityDespawn);
    }

    void SendChunkUpdates(const Packet::ChunkUpdateData& update) {
        chunk_update_queue_.Push(update);
    }
};
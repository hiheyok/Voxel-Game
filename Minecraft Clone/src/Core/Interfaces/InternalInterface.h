#pragma once
#include "ServerInterface.h"
#include "Utils/LogUtils.h"
#include "Utils/Containers/ConcurrentQueue.h"
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

    bool IsConnected() const override {
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

    void SendBlockUpdate(const Packet::BlockUpdate& update) override {
        block_update_queue_.Push(update);
    }

    void SendEntityUpdate(const Packet::EntityUpdate& update) override {
        entity_update_queue_.Push(update);
    }

    void SendChunkUpdates(const Packet::ChunkUpdateData& update) override {
        chunk_update_queue_.Push(update);
    }

    void SendServerStats(const ServerStats& stats) override {
        server_stats_ = stats;
    }
};
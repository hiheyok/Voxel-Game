#pragma once
#include <string>
#include <vector>

#include "Utils/Containers/ConcurrentQueue.h"
#include "Core/Networking/Packet.h"
#include "Core/Stats/ServerStats.h"
#include "Utils/Timer/Timer.h"
/*
* Abstraction for client to server interfacing
*/
class ServerInterface {

public:
    virtual ~ServerInterface() = default;

    virtual void SendPlayerAction(const Packet::PlayerAction& input) = 0;

    virtual bool IsConnected() const = 0;
    virtual void Connect(const std::string& address, int port) = 0; // For external server
    virtual void Disconnect() = 0;
    virtual void Update() = 0;

    int PollBlockUpdates(std::vector<Packet::BlockUpdate>& outUpdates) {
        size_t prevSize = outUpdates.size();
        block_update_queue_.PopAll(outUpdates);
        return outUpdates.size() - prevSize;
    }

    int PollEntityUpdates(std::vector<Packet::EntityUpdate>& outUpdates) {
        size_t prevSize = outUpdates.size();
        entity_update_queue_.PopAll(outUpdates);
        return outUpdates.size() - prevSize;
    }

    int PollChunkUpdates(std::vector<Packet::ChunkUpdateData>& outUpdates) {
        size_t prevSize = outUpdates.size();
        chunk_update_queue_.PopAll(outUpdates);
        return outUpdates.size() - prevSize;
    }

    ServerStats GetServerStats() {
        return server_stats_;
    }

    EntityUUID GetPlayerUUID() const {
        return client_player_uuid_;
    }

    Timer time;
protected:
    // Server -> Client queues
    ConcurrentQueue<Packet::BlockUpdate> block_update_queue_;
    ConcurrentQueue<Packet::EntityUpdate> entity_update_queue_;
    ConcurrentQueue<Packet::ChunkUpdateData> chunk_update_queue_;

    ServerStats server_stats_;
    EntityUUID client_player_uuid_;
};
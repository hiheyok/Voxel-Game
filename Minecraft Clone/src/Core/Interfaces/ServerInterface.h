#pragma once
#include <string>
#include <vector>

#include "Core/Networking/Packet.h"
#include "Core/Stats/ServerStats.h"
#include "Utils/Containers/ConcurrentQueue.h"
#include "Utils/Timer/Timer.h"
/*
 * Abstraction for client to server interfacing
 */
class ServerInterface {
   public:
    virtual ~ServerInterface() = default;

    virtual void SendPlayerAction(const Packet::PlayerAction& input) = 0;

    virtual bool IsConnected() const = 0;
    virtual void Connect(const std::string& address,
                         int port) = 0;  // For external server
    virtual void Disconnect() = 0;
    virtual void Update() = 0;

    size_t PollBlockUpdates(std::vector<Packet::BlockUpdate>& outUpdates) {
        size_t prevSize = outUpdates.size();
        block_update_queue_.PopAll(outUpdates);
        return outUpdates.size() - prevSize;
    }

    size_t PollEntityUpdates(std::vector<Packet::EntityUpdate>& outUpdates) {
        size_t prevSize = outUpdates.size();
        entity_update_queue_.PopAll(outUpdates);
        return outUpdates.size() - prevSize;
    }

    size_t PollChunkUpdates(std::vector<Packet::ChunkUpdateData>& outUpdates) {
        size_t prevSize = outUpdates.size();
        chunk_update_queue_.PopAll(outUpdates);
        return outUpdates.size() - prevSize;
    }

    ServerStats GetServerStats() { return server_stats_; }

    EntityUUID GetPlayerUUID() const { return client_player_uuid_; }

    Timer time;

   protected:
    // Server -> Client queues
    ConcurrentQueue<Packet::BlockUpdate> block_update_queue_;
    ConcurrentQueue<Packet::EntityUpdate> entity_update_queue_;
    ConcurrentQueue<Packet::ChunkUpdateData> chunk_update_queue_;

    ServerStats server_stats_;
    EntityUUID client_player_uuid_;
};
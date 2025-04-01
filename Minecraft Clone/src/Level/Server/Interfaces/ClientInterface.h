#pragma once
#include "../../../Utils/Containers/ConcurrentQueue.h"
#include "../Networking/Packet.h"

#include "../../Typenames.h"

/*
* Abstraction for server to client interfacing
*/

class ClientInterface {
public:
    virtual void SendBlockUpdate(const Packet::BlockUpdate& update) = 0;
    virtual void SendEntityUpdate(const Packet::EntityUpdate& update) = 0;
    virtual void SendChunkUpdates(const Packet::ChunkUpdateData& update) = 0;

    ClientInterface() = default;

    bool PollClientPlayerAction(std::vector<Packet::PlayerAction>& playerAction) {
        size_t prevSize = playerAction.size();
        player_action_queue_.PopAll(playerAction);
        return prevSize != playerAction.size();
    }

    EntityUUID GetPlayerUUID() const { 
        return player_uuid_; 
    }

    void SetPlayerUUID(EntityUUID playerUUID) { 
        player_uuid_ = playerUUID;
    }
protected:
    // Client -> Server queues
    ConcurrentQueue<Packet::PlayerAction> player_action_queue_;
    EntityUUID player_uuid_;
};
// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <vector>

#include "Core/DataStructure/ConcurrentQueue.h"
#include "Core/DataStructure/blockingconcurrentqueue.h"
#include "Core/Networking/Packet.h"
#include "Core/Stats/ServerStats.h"
#include "Core/Typenames.h"
/*
 * Abstraction for server to client interfacing
 */

class ClientInterface {
 public:
  virtual void SendBlockUpdate(const Packet::BlockUpdate& update) = 0;
  virtual void SendEntityUpdate(const Packet::EntityUpdate& update) = 0;
  virtual void SendChunkUpdates(const Packet::ChunkUpdateData& update) = 0;
  virtual void SendECSUpdate(const ECSUpdatePacket::ECSUpdate& update) = 0;
  virtual void SendServerStats(const ServerStats& stats) = 0;

  virtual void SendTimeLastTick() = 0;

  ClientInterface() = default;
  virtual ~ClientInterface() = default;

  size_t PollClientPlayerAction(
      std::vector<Packet::PlayerAction>& out_updates) {
    const int queue_size = player_action_queue_.size_approx();
    if (queue_size == 0) return 0;
    std::vector<Packet::PlayerAction> temp(queue_size);

    size_t dequeue_count =
        player_action_queue_.wait_dequeue_bulk(temp.data(), queue_size);
    out_updates.insert(out_updates.end(), std::make_move_iterator(temp.begin()),
                       std::make_move_iterator(temp.begin() + dequeue_count));
    return dequeue_count;
  }

  EntityUUID GetPlayerUUID() const { return player_uuid_; }

  void SetPlayerUUID(EntityUUID playerUUID) { player_uuid_ = playerUUID; }

 protected:
  // Client -> Server queues
  moodycamel::BlockingConcurrentQueue<Packet::PlayerAction>
      player_action_queue_;
  EntityUUID player_uuid_;

};

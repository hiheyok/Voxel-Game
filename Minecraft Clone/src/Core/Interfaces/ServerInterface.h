// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <string>
#include <vector>

#include "Core/DataStructure/ConcurrentQueue.h"
#include "Core/DataStructure/blockingconcurrentqueue.h"
#include "Core/Networking/Packet.h"
#include "Core/Stats/ServerStats.h"
#include "Utils/Timer/Timer.h"
/*
 * Abstraction for client to server interfacing
 */
class ServerInterface {
 public:
  ServerInterface() = default;
  virtual ~ServerInterface() = default;

  virtual void SendPlayerAction(const Packet::PlayerAction& input) = 0;

  virtual bool IsConnected() const = 0;
  virtual void Connect(const std::string& address,
                       int port) = 0;  // For external server
  virtual void Disconnect() = 0;
  virtual void Update() = 0;

  size_t PollBlockUpdates(std::vector<Packet::BlockUpdate>& out_updates) {
    const int queue_size = block_update_queue_.size_approx();
    if (queue_size == 0) return 0;
    std::vector<Packet::BlockUpdate> temp(queue_size);

    size_t dequeue_count =
        block_update_queue_.wait_dequeue_bulk(temp.data(), queue_size);

    out_updates.insert(out_updates.end(), std::make_move_iterator(temp.begin()),
                       std::make_move_iterator(temp.begin() + dequeue_count));
    return dequeue_count;
  }

  size_t PollEntityUpdates(std::vector<Packet::EntityUpdate>& out_updates) {
    const int queue_size = entity_update_queue_.size_approx();
    if (queue_size == 0) return 0;
    std::vector<Packet::EntityUpdate> temp(queue_size);

    size_t dequeue_count =
        entity_update_queue_.wait_dequeue_bulk(temp.data(), queue_size);

    out_updates.insert(out_updates.end(), std::make_move_iterator(temp.begin()),
                       std::make_move_iterator(temp.begin() + dequeue_count));
    return dequeue_count;
  }

  size_t PollChunkUpdates(std::vector<Packet::ChunkUpdateData>& out_updates) {
    const int queue_size = chunk_update_queue_.size_approx();
    if (queue_size == 0) return 0;
    
    std::vector<Packet::ChunkUpdateData> temp(queue_size);

    size_t dequeue_count =
        chunk_update_queue_.wait_dequeue_bulk(temp.data(), queue_size);

    out_updates.insert(out_updates.end(), std::make_move_iterator(temp.begin()),
                       std::make_move_iterator(temp.begin() + dequeue_count));
    return dequeue_count;
  }

  ServerStats GetServerStats() { return server_stats_; }

  EntityUUID GetPlayerUUID() const { return client_player_uuid_; }

  Timer time;

 protected:
  // Server -> Client queues
  moodycamel::BlockingConcurrentQueue<Packet::BlockUpdate> block_update_queue_;
  moodycamel::BlockingConcurrentQueue<Packet::EntityUpdate>
      entity_update_queue_;
  moodycamel::BlockingConcurrentQueue<Packet::ChunkUpdateData>
      chunk_update_queue_;

  ServerStats server_stats_;
  EntityUUID client_player_uuid_;

};

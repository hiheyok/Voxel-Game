// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <algorithm>
#include <glm/vec3.hpp>
#include <vector>

#include "Core/Typenames.h"

struct DrawCommandIndirect {
  uint32_t count_;
  uint32_t instance_count_;
  uint32_t first_;
  uint32_t base_instance_;

  DrawCommandIndirect()
      : count_{0}, instance_count_{0}, first_{0}, base_instance_{0} {}

  DrawCommandIndirect(uint32_t count, uint32_t instanceCount, uint32_t first,
                      uint32_t baseInstance)
      : count_{count},
        instance_count_{instanceCount},
        first_{first},
        base_instance_{baseInstance} {}

  void set(uint32_t count, uint32_t instanceCount, uint32_t first,
           uint32_t baseInstance) {
    count_ = count;
    instance_count_ = instanceCount;
    first_ = first;
    base_instance_ = baseInstance;
  }
};

namespace CMDGraph {

const int NX = 0x00;
const int PX = 0x01;
const int NY = 0x02;
const int PY = 0x03;
const int NZ = 0x04;
const int PZ = 0x05;

struct CommandPtr {
  CommandPtr* neighbor_[6]{nullptr, nullptr, nullptr,
                           nullptr, nullptr, nullptr};
  DrawCommandIndirect cmd_;
  ChunkPos position_;
  int visit_id_ = 0;

  CommandPtr() {
    for (const auto& side : Directions<ChunkPos>()) {
      neighbor_[side] = nullptr;
    }
  }

  explicit CommandPtr(DrawCommandIndirect cmd) : cmd_(cmd) {
    for (const auto& side : Directions<ChunkPos>()) {
      neighbor_[side] = nullptr;
    }
  }

  CommandPtr(DrawCommandIndirect cmd, ChunkPos pos)
      : cmd_(cmd), position_(pos) {
    for (const auto& side : Directions<ChunkPos>()) {
      neighbor_[side] = nullptr;
    }
  }

  ~CommandPtr() = default;

  void PrepareDelete() {
    for (int i = 0; i < 3; i++) {
      int index1 = i * 2;
      int index2 = i * 2 + 1;

      if ((neighbor_[index1] != nullptr) && (neighbor_[index2] != nullptr)) {
        neighbor_[index1]->neighbor_[index2] = neighbor_[index2];
        neighbor_[index2]->neighbor_[index1] = neighbor_[index1];
      } else {
        if ((neighbor_[index1] != nullptr)) {
          neighbor_[index1]->neighbor_[index2] = nullptr;
        }
        if ((neighbor_[index2] != nullptr)) {
          neighbor_[index2]->neighbor_[index1] = nullptr;
        }
      }
    }
  }

  float GetSquareDistance(float px, float py, float pz) {
    return (px - position_.x) * (px - position_.x) +
           (py - position_.y) * (py - position_.y) +
           (pz - position_.z) * (pz - position_.z);
  }

  float GetSquareDistance(ChunkPos other) {
    return GetSquareDistance(static_cast<float>(other.x),
                             static_cast<float>(other.y),
                             static_cast<float>(other.z));
  }

  float GetSquareDistance(const glm::vec3& other) {
    return GetSquareDistance(other.x, other.y, other.z);
  }

  void SetNeighbor(CommandPtr* node, int side, int depth = 0) {
    // Early exit if trying to set a neighbor to itself
    if (node == this) return;

    if ((neighbor_[side] == nullptr) && (node->neighbor_[!side] == nullptr)) {
      neighbor_[side] = node;
      node->neighbor_[!side] = this;
      return;
    }

    CommandPtr* p1 = this;
    CommandPtr* p2 = node;
    CommandPtr* p3 = neighbor_[side];
    CommandPtr* p4 = node->neighbor_[!side];

    // Temp set to null
    neighbor_[side] = nullptr;
    if (p3 != nullptr) {
      p3->neighbor_[!side] = nullptr;
    }
    node->neighbor_[!side] = nullptr;
    if (p4 != nullptr) {
      p4->neighbor_[side] = nullptr;
    }

    if ((p4 != nullptr) && (p3 == nullptr)) {
      int side_p4 = p1->GetSideLocation(p4->position_);

      if (side_p4 == side) {  // OK
        p1->SetNeighbor(p4, side, depth + 1);

        p4->neighbor_[side] = p2;
        p2->neighbor_[!side] = p4;
      } else {
        p1->SetNeighbor(p4, side_p4, depth + 1);

        p1->neighbor_[side] = p2;
        p2->neighbor_[!side] = p1;
      }
      return;
    }

    if ((p4 == nullptr) && (p3 != nullptr)) {
      // first check if p4 or p3 is closer to p1

      float p1_distance_p3 = p1->GetSquareDistance(p3->position_);
      float p1_distance_p2 = p1->GetSquareDistance(p2->position_);

      if (p1_distance_p3 <= p1_distance_p2) {  // this implies that p3 is the
                                               // closest is is kept the same

        // reconnects p3 and p1
        p1->neighbor_[side] = p3;
        p3->neighbor_[!side] = p1;

        int side_p2 = p3->GetSideLocation(p2->position_);
        p3->SetNeighbor(p2, side_p2, depth + 1);

      } else {
        p1->neighbor_[side] = p2;  // no data lost occur here since if p4 is
                                   // null, it implies p2 neighbor is null
        p2->neighbor_[!side] = p1;

        int side_p3 = p2->GetSideLocation(p3->position_);
        p2->SetNeighbor(p3, side_p3, depth + 1);
      }
      return;
    }

    // return;
    // last case (p4 != nullptr) && (p3 != nullptr)

    float p1_distance_p3 = p1->GetSquareDistance(p3->position_);
    float p1_distance_p2 = p1->GetSquareDistance(p2->position_);
    float p1_distance_p4 = p1->GetSquareDistance(p4->position_);

    if ((p1_distance_p3 <= p1_distance_p2) &&
        (p1_distance_p3 <= p1_distance_p4)) {  // p3 is closest to p1 //OK
      // reconnects p3 and p1
      p1->neighbor_[side] = p3;
      p3->neighbor_[!side] = p1;

      // connect p4 to p3 at side p3_side_p4
      int p3_side_p4 = p3->GetSideLocation(p4->position_);
      p3->SetNeighbor(p4, p3_side_p4, depth + 1);

      p4->neighbor_[side] = p2;
      p2->neighbor_[!side] = p4;

      return;
    }

    if ((p1_distance_p4 <= p1_distance_p3) &&
        (p1_distance_p4 <= p1_distance_p2)) {  // p4 is closest
      // connect p4 and p1
      p1->SetNeighbor(p4, side, depth + 1);

      if (p1_distance_p3 >= p1_distance_p2) {
        int p4_side_p2 = p4->GetSideLocation(p2->position_);

        if (p4_side_p2 == side) {
          p4->SetNeighbor(p2, p4_side_p2, depth + 1);

          int p2_side_p3 = p2->GetSideLocation(p3->position_);
          p2->SetNeighbor(p3, p2_side_p3, depth + 1);

        } else {
          p4->SetNeighbor(p2, p4_side_p2, depth + 1);
          int p4_side_p3 = p4->GetSideLocation(p3->position_);
          p4->SetNeighbor(p3, p4_side_p3, depth + 1);
        }
      }

      return;
    }

    if ((p1_distance_p2 <= p1_distance_p3) &&
        (p1_distance_p2 <=
         p1_distance_p4)) {  // if p2 is closer, it implies p4 is behind p1
      // connect p4 and p1
      int p1_side_p4 = p1->GetSideLocation(p4->position_);
      p1->SetNeighbor(p4, p1_side_p4, depth + 1);

      // connect p2 to p1
      p1->SetNeighbor(p2, side, depth + 1);

      // connect p3 to p2
      int p2_side_p3 = p2->GetSideLocation(p3->position_);
      p2->SetNeighbor(p3, p2_side_p3, depth + 1);
    }
  }

  int GetSideLocation(ChunkPos p2) const {
    ChunkPos relativePosition = p2 - position_;

    int x = relativePosition.x;
    int y = relativePosition.y;
    int z = relativePosition.z;

    if ((abs(x) >= abs(z)) && (abs(x) >= abs(y))) {
      if (x >= 0) {
        return PX;
      } else {
        return NX;
      }
    }
    if ((abs(z) >= abs(x)) && (abs(z) >= abs(y))) {
      if (z >= 0) {
        return PZ;
      } else {
        return NZ;
      }
    }
    if ((abs(y) >= abs(x)) && (abs(y) >= abs(z))) {
      if (y >= 0) {
        return PY;
      } else {
        return NY;
      }
    }
    g_logger.LogError("CommandPtr::GetSideLocation", "Error");
  }
};
}  // namespace CMDGraph

class ChunkDrawCommandBuffer {
 public:
  void Initialize() {}

  void AddDrawCommand(DrawCommandIndirect cmd, ChunkPos pos) {
    CMDGraph::CommandPtr* ptr = new CMDGraph::CommandPtr(cmd, pos);
    command_map_[pos] = ptr;
    AddToGraph(ptr);
  }

  void UpdateCurrentPosition(glm::vec3 NewPosition) {
    current_position_ = NewPosition;
  }

  size_t GetDrawCommandsSorted(std::vector<DrawCommandIndirect>& cmds,
                               std::vector<int>& cmdPos) {
    for (auto& arr : commands_) {
      arr.clear();
    }

    for (auto& arr : commands_position_) {
      arr.clear();
    }
    count = 0;
    size_t size = 0;
    TraverseTree(root_, 0);

    current_node_visit_id_++;

    if (current_node_visit_id_ == kChunkDim) {
      current_node_visit_id_ = 0;
    }

    for (int64_t i = static_cast<int64_t>(commands_.size()) - 1; i >= 0; i--) {
      size += commands_[i].size();
      cmds.insert(cmds.end(), commands_[i].begin(), commands_[i].end());
      cmdPos.insert(cmdPos.end(), commands_position_[i].begin(),
                    commands_position_[i].end());
    }

    for (int i = 0; i < size; i++) {
      cmds.at(i).base_instance_ = i + 1;
    }

    // std::cout << count << ", " << CommandMap.size() << "\n";

    return size;
  }

  void DeleteDrawCommand(ChunkPos pos) {
    CMDGraph::CommandPtr* node = command_map_[pos];
    command_map_.erase(pos);

    if (node != root_) {
      node->PrepareDelete();
      delete node;
      return;
    }

    float distances[6]{0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

    CMDGraph::CommandPtr* rootNeighbors[6]{nullptr};

    for (const auto& side : Directions<ChunkPos>()) {
      rootNeighbors[side] = node->neighbor_[side];
    }
    node->PrepareDelete();
    delete node;

    for (const auto& side : Directions<ChunkPos>()) {
      if (rootNeighbors[side] == nullptr) {
        continue;
      }

      distances[side] =
          rootNeighbors[side]->GetSquareDistance(current_position_);
    }

    float min = FLT_MAX;

    for (const auto& side : Directions<ChunkPos>()) {
      if (distances[side] == 0.f) {
        continue;
      }

      min = std::min(min, distances[side]);
    }

    if (min == FLT_MAX) {
      root_ = nullptr;
      return;
    }

    for (const auto& side : Directions<ChunkPos>()) {
      if (distances[side] == min) {
        root_ = rootNeighbors[side];
        break;
      }
    }
  }

  int count = 0;

  void TraverseTree(CMDGraph::CommandPtr* node, int depth) {
    if (node == nullptr) {
      return;
    }

    if (node->visit_id_ == current_node_visit_id_) {
      return;
    }

    node->visit_id_ = current_node_visit_id_;

    if (commands_.size() <= depth) {
      commands_.emplace_back();
      commands_position_.emplace_back();
      commands_layer_.emplace_back();
    }

    commands_[depth].push_back(node->cmd_);
    commands_position_[depth].push_back(node->position_.x);
    commands_position_[depth].push_back(node->position_.y);
    commands_position_[depth].push_back(node->position_.z);
    //    CommandsLayer[depth].push_back(depth);
    count++;
    for (const auto& side : Directions<ChunkPos>()) {
      TraverseTree(node->neighbor_[side], depth + 1);
    }
  }

 private:
  bool VerifyTree(CMDGraph::CommandPtr* node) {  // debug
    bool isCorrect = true;

    if (node->visit_id_ == current_node_visit_id_) {
      return isCorrect;
    }

    for (const auto& side : Directions<ChunkPos>()) {
      if (node->neighbor_[side] == nullptr) {
        continue;
      }

      if (node->neighbor_[side]->neighbor_[!side] != node) {
        return false;
      } else {
        if (node->neighbor_[side]->visit_id_ != current_node_visit_id_) {
          isCorrect = VerifyTree(node->neighbor_[side]);
        }
      }
    }

    node->visit_id_ = current_node_visit_id_;

    return isCorrect;
  }

  std::vector<std::vector<DrawCommandIndirect>>
      commands_;  // [Tree Depth][Draw Commands]
  std::vector<std::vector<int>>
      commands_position_;  // [Tree Depth][Draw Commands Position]
  std::vector<std::vector<int>> commands_layer_;  // [Tree Depth][Layer]

  int current_node_visit_id_ = 1;

  void AddToGraph(CMDGraph::CommandPtr* node) {
    // If root doesn't exist add to root
    if (root_ == nullptr) {
      root_ = node;
      return;
    }

    // If root exist, check if the chunk is closer to the actual root if so
    // change the root

    if (root_->GetSquareDistance(current_position_) >
        node->GetSquareDistance(current_position_)) {
      int loc = node->GetSideLocation(root_->position_);

      CMDGraph::CommandPtr* tmp = root_;
      root_ = node;
      node->SetNeighbor(tmp, loc);
      return;
    }

    CMDGraph::CommandPtr* curr = root_;

    while (true) {
      int side = curr->GetSideLocation(node->position_);

      if (curr->neighbor_[side] == nullptr) {
        curr->SetNeighbor(node, side);
        return;
      }

      if (curr->neighbor_[side]->GetSquareDistance(node->position_) >=
          curr->GetSquareDistance(node->position_)) {
        curr->SetNeighbor(node, side);
        return;
      }

      curr = curr->neighbor_[side];
    }
  }

  glm::vec3 current_position_ = glm::vec3(0.f, 0.f, 0.f);

  CMDGraph::CommandPtr* root_ = nullptr;
  FastHashMap<ChunkPos, CMDGraph::CommandPtr*> command_map_;
};

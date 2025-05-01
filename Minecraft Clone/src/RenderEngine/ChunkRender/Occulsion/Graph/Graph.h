// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <list>

template <class T>
struct Node {
  T* vertex_;
  Node* edge_;
};

template <class T>
class Graph {
 public:
  void InsertVertex(T* vertex) {}

  void InsertEdge(T* vertex0, T* vertex1) {}

  void DeleteEdge() {}

  void DeleteVertex(T* vertex) {}

 private:
  std::list<Node> nodes_;
};

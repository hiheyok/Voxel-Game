// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>
#include <vector>

#include "Client/ClientLevel/ClientCache.h"

struct ClientLevel {
  explicit ClientLevel(GameContext&);
  ~ClientLevel();

  void Update();

  ClientCache cache_;
};

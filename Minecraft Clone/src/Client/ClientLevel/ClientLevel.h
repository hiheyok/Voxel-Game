// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Client/ClientLevel/ClientCache.h"

struct ClientLevel {
  explicit ClientLevel(GameContext&);
  ~ClientLevel();

  ClientCache cache_;
};

#pragma once
#include "Client/ClientLevel/ClientCache.h"

struct ClientLevel {
  ClientLevel() = default;
  ~ClientLevel() = default;

  ClientCache cache;
};
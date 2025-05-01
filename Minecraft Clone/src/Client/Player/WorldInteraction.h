#pragma once
#include "Core/Typenames.h"

class Player;
class UserInputs;
class ServerInterface;
class ClientCache;

struct Ray;

class WorldInteraction {
 public:
  void Interact(Player* player, const UserInputs& Inputs,
                ServerInterface* interface, ClientCache* cache);

 private:
  BlockID GetBlock(Ray ray, ServerInterface* interface, ClientCache* cache);
  void BreakBlock(Ray ray, ServerInterface* interface, ClientCache* cache);
  void PlaceBlock(Ray ray, BlockID block, ServerInterface* interface,
                  ClientCache* cache);
};
// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

class Player;
class InputManager;
class ClientCache;

class PlayerMovement {
 public:
  void Update(Player* player, const InputManager& inputs, ClientCache* world);

 private:
  float VelocityMovementCurve(float current, float max, float delta);

  void RotatePlayer(Player* player, const InputManager& inputs);
  void MovePlayer(Player* player, const InputManager& inputs,
                  ClientCache* server);
  void MoveRelative(Player* player, float strafe, float up, float forward,
                    float friction);

  bool enable_collusion_ = false;
};

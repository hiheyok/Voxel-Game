// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>

class ClientCache;
class ClientActionQueue;
class WorldInteraction;
class InputManager;
class PlayerMovement;
class UIManager;

class PlayerPOV;
class Player;
class GameContext;

struct EntityProperty;

class MainPlayer {
 public:
  MainPlayer(GameContext&, ClientCache& cache, UIManager& ui_manager);
  MainPlayer(const MainPlayer&) = delete;
  ~MainPlayer();

  void Update(const InputManager& inputs, ClientActionQueue& action_queue);

  EntityProperty GetEntityProperties();

  void SetPlayerRotation(float x, float y);
  void SetPlayerPosition(float x, float y, float z);

  PlayerPOV* GetPlayerPOV();

  GameContext& context_;
  std::unique_ptr<Player> player_;

 private:
  static constexpr int kHotbarSlots = 9;
  void InventoryUpdate(const InputManager& inputs);
  void SetupUITickCallback();

  std::unique_ptr<PlayerMovement> movement_;
  std::unique_ptr<WorldInteraction> interactions_;
  std::unique_ptr<PlayerPOV> player_pov_;

  UIManager& ui_manager_;
  ClientCache& client_cache_;
};

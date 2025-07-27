// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>

class ClientCache;
class ServerInterface;
class WorldInteraction;
class UserInputs;
class PlayerMovement;
class PlayerPOV;
class Player;
class GUI;
class Window;
class GameContext;

struct EntityProperty;

class MainPlayer {
 public:
  MainPlayer(GameContext&, Window* window, ServerInterface* server,
             ClientCache* cache);
  MainPlayer(const MainPlayer&) = delete;
  ~MainPlayer();

  void Update(const UserInputs& inputs);

  EntityProperty GetEntityProperties();

  void SetPlayerRotation(float x, float y);
  void SetPlayerPosition(float x, float y, float z);

  PlayerPOV* GetPlayerPOV();

  void RenderGUIs();

  GameContext& game_context_;
  std::unique_ptr<Player> player_;

 private:
  static constexpr int kHotbarSlots = 9;
  void PrepareGUIs();
  void InventoryUpdate(const UserInputs& inputs);

  std::unique_ptr<PlayerMovement> movement_;
  std::unique_ptr<WorldInteraction> interactions_;
  std::unique_ptr<PlayerPOV> player_pov_;
  std::unique_ptr<GUI> player_gui_;

  ServerInterface* internal_interface_ = nullptr;
  ClientCache* client_cache_ = nullptr;

  size_t gui_index_ = 0;
  size_t item_gui_index_ = 0;
  size_t slot_index_ = 0;

  static constexpr float kHotbarSize = 0.135f;
};

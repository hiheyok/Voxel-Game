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

struct GLFWwindow;
struct EntityProperty;


class MainPlayer {
public:

    MainPlayer();
    ~MainPlayer();

    void Initialize(GLFWwindow* win, ServerInterface* server, ClientCache* cache);

    void Update(const UserInputs& inputs);

    EntityProperty GetEntityProperties();

    void SetPlayerRotation(float x, float y);
    void SetPlayerPosition(float x, float y, float z);

    PlayerPOV* GetPlayerPOV();

    void RenderGUIs();

    std::unique_ptr<Player> player_;
private:

    void PrepareGUIs();
    void InventoryUpdate(const UserInputs& inputs);

    std::unique_ptr<PlayerMovement> movement_;
    std::unique_ptr<WorldInteraction> interactions_;
    std::unique_ptr<PlayerPOV> player_pov_;
    std::unique_ptr<GUI> player_gui_;

    ClientCache* client_cache_ = nullptr;
    ServerInterface* internal_interface_ = nullptr;

    size_t gui_index_ = 0;
    size_t item_gui_index_ = 0;
    size_t slot_index_ = 0;

    constexpr const static float kHotbarSize = 0.135f;
};
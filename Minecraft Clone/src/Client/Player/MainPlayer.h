#pragma once
#include <memory>

class InternalServer;
class UserInputs;
class Dimension;
class PlayerMovement;
class Player;
class WorldInteraction;
class GUI;

struct GLFWwindow;
struct EntityProperty;


class MainPlayer {
public:

    MainPlayer();
    ~MainPlayer();

    void Initialize(GLFWwindow* win, InternalServer* server);

    void Update(UserInputs inputs, Dimension* dimension);

    EntityProperty GetEntityProperties();

    void SetPlayerRotation(float x, float y);
    void SetPlayerPosition(float x, float y, float z);

    void RenderGUIs();

    std::unique_ptr<Player> player_;
private:

    void PrepareGUIs();
    void InventoryUpdate(UserInputs inputs);

    std::unique_ptr<PlayerMovement> movement_;
    std::unique_ptr<WorldInteraction> interactions_;
    std::unique_ptr<GUI> player_gui_;

    InternalServer* internal_server_ = nullptr;

    size_t gui_index_ = 0;
    size_t item_gui_index_ = 0;
    size_t slot_index_ = 0;

    float hotbar_size_ = 0.135;
};
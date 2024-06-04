#pragma once
#include "../../Level/Item/Inventory/PlayerInventory.h"
#include "WorldInteraction.h"
#include "PlayerMovement.h"
#include "../IO/IO.h"
#include "../../Level/Entity/Mobs/Player.h"
#include "../../RenderEngine/GUI/GUI.h"

class MainPlayer {
public:

	void Initialize(GLFWwindow* win, InternalServer* server);

	void Update(UserInputs Inputs);

	EntityProperty GetEntityProperties();

	void SetPlayerRotation(float x, float y);

	void SetPlayerPosition(float x, float y, float z);

	void RenderGUIs();

	Player m_Player;
private:

	void PrepareGUIs();

	void InventoryUpdate(UserInputs Inputs);

	PlayerMovement m_Movement;
	WorldInteraction m_Interactions;
	InternalServer* internalServer;

	GUI PlayerGUI;

	int GUIIndex = 0;
	int ItemGUIIndex = 0;
	int SlotIndex = 0;

	float HotbarSize = 0.135;

};
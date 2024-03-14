#pragma once
#include "../../World/Item/Inventory/PlayerInventory.h"
#include "WorldInteraction.h"
#include "PlayerMovement.h"
#include "../IO/IO.h"
#include "../../World/Entity/Mobs/Player.h"
#include "../../RenderEngine/GUI/GUI.h"

class MainPlayer {
public:

	void Initialize(GLFWwindow* win);

	void Update(UserInputs Inputs);

	EntityProperty GetEntityProperties();

	void SetPlayerRotation(float x, float y);

	void SetPlayerPosition(float x, float y, float z);

	void RenderGUIs();

private:

	void PrepareGUIs();

	void InventoryUpdate(UserInputs Inputs);

	PlayerMovement m_Movement;
	WorldInteraction m_Interactions;
	Player m_Player;

	GUI PlayerGUI;

	int GUIIndex = 0;
	int ItemGUIIndex = 0;
	int SlotIndex = 0;

};
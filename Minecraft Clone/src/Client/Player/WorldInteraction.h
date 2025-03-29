#pragma once
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include "../../Level/Event/EventHandler.h"
#include <glm/gtx/common.hpp>
#include "../../Utils/Math/vectorOperations.h"
#include "../IO/IO.h"
#include "../IO/KEY_CODE.h"
#include "../../Level/Entity/Entity.h"
#include "../../Level/Entity/Mobs/Player.h"
#include "../../Level/Entity/Properties/EntityProperties.h"
#include "../../Level/Server/Communication/InternalServer.h"

class WorldInteraction {
public:
    void Interact(Player* player, const UserInputs& Inputs, Dimension* dimension);
private:
    BlockID GetBlock(Ray ray, Dimension* dimension);
    void BreakBlock(Ray ray, Dimension* dimension);
    void PlaceBlock(Ray ray, BlockID block, Dimension* dimension);
};
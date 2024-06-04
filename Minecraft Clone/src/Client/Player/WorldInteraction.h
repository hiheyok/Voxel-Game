#pragma once
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include "../../World/Event/EventHandler.h"
#include <glm/gtx/common.hpp>
#include "../../Utils/Math/vectorOperations.h"
#include "../IO/IO.h"
#include "../IO/KEY_CODE.h"
#include "../../World/Entity/Entity.h"
#include "../../World/Entity/Mobs/Player.h"
#include "../../World/Entity/Properties/EntityProperties.h"
#include "../../Level/Server/Communication/InternalServer.h"

using namespace glm;

class WorldInteraction {

public:

	void Interact(Player* player, UserInputs Inputs);

private:

	BlockID GetBlock(Ray ray, Dimension* dimension);

	void BreakBlock(Ray ray, Dimension* dimension);

	void PlaceBlock(Ray ray, BlockID block, Dimension* dimension);

};
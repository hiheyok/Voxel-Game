#pragma once
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include "../../World/Event/EventHandler.h"
#include <glm/gtx/common.hpp>
#include "../../Utils/Math/vectorOperations.h"
#include "../IO/IO.h"
#include "../IO/KEY_CODE.h"
#include "../../World/Entity/Entity.h"
#include "../../World/World.h"
#include "../../World/Entity/Entities.h"
#include "../../World/Entity/Properties/EntityProperties.h"

using namespace glm;



class WorldInteraction {

public:

	void Interact(Entity* Player, UserInputs Inputs) {
		Ray ray;
		ray.Origin = Player->Properties.Position;
		ray.Direction = dvec3(
			cos(Player->Properties.Rotation.x * 0.0174533) * cos(Player->Properties.Rotation.y * 0.0174533),
			sin(Player->Properties.Rotation.y * 0.0174533),
			sin(Player->Properties.Rotation.x * 0.0174533) * cos(Player->Properties.Rotation.y * 0.0174533));

		World* world = static_cast<World*>(Block::WorldPTR);

		BlockID block = Player->m_EntityInventory.GetItem(Player->m_EntityInventory.RightHandSlot).m_item.GetBlock();

		if (Inputs.Mouse.RIGHT == Inputs.Mouse.PRESS) {
			PlaceBlock(ray, block, world);
		}

		if (Inputs.Mouse.LEFT == Inputs.Mouse.PRESS) {
			BreakBlock(ray, world);
		}
		if (Inputs.Mouse.MIDDLE == Inputs.Mouse.PRESS) {
			BlockID newBlock = GetBlock(ray, world);
			Player->m_EntityInventory.SetSlot(Player->m_EntityInventory.RightHandSlot, ItemStack(Items.GetItem(Items.GetBlockItem(newBlock))));
		}
	}

private:

	BlockID GetBlock(Ray ray, World* world) {
		if (world->RayIntersection(ray)) {
			return world->GetBlock((int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z));
		}
	}

	void BreakBlock(Ray ray, World* world) {
		if (world->RayIntersection(ray)) {
			Event placeBlock;
			placeBlock.Type = BLOCK_EVENT;
			placeBlock.Data.BlockEvent.block = Blocks.AIR;
			placeBlock.Data.BlockEvent.x = (int)floor(ray.EndPoint.x);
			placeBlock.Data.BlockEvent.y = (int)floor(ray.EndPoint.y);
			placeBlock.Data.BlockEvent.z = (int)floor(ray.EndPoint.z);
			placeBlock.Data.BlockEvent.id = EventHandler.BlockPlace;
			world->QueueEvent(placeBlock);
		}
	}

	void PlaceBlock(Ray ray, BlockID block, World* world) {

		if (world->RayIntersection(ray)) {

			int BounceSurface = ray.bouncesurface;

			ivec3 PlacePos = floor(ray.EndPoint);

			PlacePos[(int)floor(BounceSurface >> 1)] += (BounceSurface & 0b1) - ((BounceSurface + 1) & 0b1); //Offsets block location to be placed by 1 block

			Event placeBlock;
			placeBlock.Type = BLOCK_EVENT;
			placeBlock.Data.BlockEvent.block = block;
			placeBlock.Data.BlockEvent.x = PlacePos.x;
			placeBlock.Data.BlockEvent.y = PlacePos.y;
			placeBlock.Data.BlockEvent.z = PlacePos.z;
			placeBlock.Data.BlockEvent.id = EventHandler.BlockPlace;

			world->QueueEvent(placeBlock);
		}

	}

};
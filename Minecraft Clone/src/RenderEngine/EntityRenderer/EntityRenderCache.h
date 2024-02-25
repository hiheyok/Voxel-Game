#pragma once

#include <unordered_map>
#include "../../World/Entity/Entity.h"

struct EntityRenderCache { // stores all entity to be rendered

	std::unordered_map<EntityUUID, Entity> EntityContainer;

	void AddEntity(Entity entity) {
		EntityContainer[entity.EntityUUID] = entity;
	}

	void RemoveEntity(EntityUUID EntityUUID) {
		EntityContainer.erase(EntityUUID);
	}

	std::unordered_map<EntityTypeID, std::vector<Entity>> GetEntitiesTypeSeparated() {
		std::unordered_map<EntityTypeID, std::vector<Entity>> out;
		
		for (auto& e : EntityContainer) {
			EntityTypeID type = e.second.Type;

			if (!out.count(type)) {
				out[type] = std::vector<Entity>();
			}

			out[type].push_back(e.second);
		}

		return out;
	}

};
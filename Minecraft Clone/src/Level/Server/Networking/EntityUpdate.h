#pragma once
#include <glm/vec3.hpp>

#include "../../Typenames.h"
#include "../../Entity/Properties/EntityProperties.h"
#include "../../Item/ItemStack.h"

namespace EntityUpdatePacket {
    struct EntitySpawn {
        EntitySpawn() = default;
        EntityProperty properties_;
    };

    struct EntityMove {
        EntityMove() = default;
        EntityProperty properties_;
    };

    struct EntityInventoryUpdate {
        EntityInventoryUpdate() = default;
        EntityUUID uuid;
        ItemStack item_;
        int slot_;
    };

    struct EntityDespawn {
        EntityDespawn() = default;
        EntityUUID uuid_;
    };
}
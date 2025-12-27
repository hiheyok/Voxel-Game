#pragma once

#include "Level/ECS/Components/MovementComponent.h"
#include "Level/ECS/EntitySystem.h"

/**
 * Store desire movement direction and speed
 * Store movement mode
 * Convert movement intent to velocity changes
 * Apply movement speed modifiers
 * Handle different physics mode (water / air)
 * Integrate with pathfinding system
 */

class MovementSystem : public EntitySystem<MovementComponen> {};

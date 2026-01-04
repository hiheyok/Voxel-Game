#include "Level/ECS/Components/TransformComponent.h"

#include <glm/vec3.hpp>

using glm::vec3;

TransformComponent::TransformComponent()
    : position_{0.0}, velocity_{0.0}, acceleration_{0.0} {}

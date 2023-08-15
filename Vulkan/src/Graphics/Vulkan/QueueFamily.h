#pragma once
#define GLM_FORCE_RADIANS
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete();
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
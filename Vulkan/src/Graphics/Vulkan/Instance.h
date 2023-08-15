#pragma once
#define GLM_FORCE_RADIANS
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "QueueFamily.h"

class GraphicsInstance {
public:

	//Initialize Vulkan
	//Takes in appname and vulkan version
	
	bool Initialize(const char* Name, const char* EngineName, uint32_t API_VERSION);
	void cleanup();

	VkInstance m_instance;
	VkDebugUtilsMessengerEXT debugMessenger;

private:
	//Main Functions
	bool CreateInstance(const char* Name, const char* EngineName, uint32_t API_VERSION);
	void pickPhysicalDevice();
	

	//Helper Functions
	bool checkValidationLayerSupport();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	bool isDeviceSuitable(VkPhysicalDevice device);

	std::vector<const char*> getRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
};
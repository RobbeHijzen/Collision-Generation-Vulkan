#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

constexpr auto PI = 3.14159265358979323846f;

const uint32_t WIDTH{ 800 };
const uint32_t HEIGHT{ 600 };

#ifdef NDEBUG
const bool enableValidationLayers{ false };
#else
const bool enableValidationLayers{ true };
#endif

#include <vector>
#include <fstream>

VkResult CreateDebugUtilsMessengerEXT
(
	VkInstance m_Instance, 
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
	const VkAllocationCallbacks* pAllocator, 
	VkDebugUtilsMessengerEXT* pDebugMessenger
);

void DestroyDebugUtilsMessengerEXT(VkInstance m_Instance, VkDebugUtilsMessengerEXT m_DebugMessenger, const VkAllocationCallbacks* pAllocator);

std::vector<char> ReadFile(const std::string& filename);
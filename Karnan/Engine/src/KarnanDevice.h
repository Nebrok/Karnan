#pragma once
#include "KarnanWindow.h"

#include "vulkan/vulkan.h"


//std libraries
#include <vector>

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	uint32_t graphicsFamily;
	uint32_t presentFamily;
	bool graphicsFamilyHasValue = false;
	bool presentFamilyHasValue = false;
	bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

class KarnanDevice
{
public:
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

private:
	KarnanWindow& _window;


	// Validation Layers and Instance creation
	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debugMessenger;

	// Physical Device
	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties _properties;
	VkPhysicalDeviceFeatures _supportedFeatures;

	VkDevice _device;
	VkQueue _graphicsQueue;
	VkQueue _presentQueue;

	VkSurfaceKHR _surface;


	const std::vector<char const*> _validationLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> _deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

public:
	KarnanDevice(KarnanWindow& window);
	~KarnanDevice();

	VkDevice Device() { return _device; }
	VkSurfaceKHR Surface() { return _surface; }
	QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(_physicalDevice); }


	SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(_physicalDevice); }


private:
	void CreateInstance();
	void SetupDebugMessenger();

	// Validation Layer and instance creation helpers
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);


	//Device Creation
	void PickPhysicalDevice();
	void CreateLogicalDevice();

	bool IsDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	void CreateSurface();


};
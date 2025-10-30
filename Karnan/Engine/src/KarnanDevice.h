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

	VkCommandPool _commandPool;


	const std::vector<char const*> _validationLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> _deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

public:
	KarnanDevice(KarnanWindow& window);
	~KarnanDevice();


	// Not copyable or movable
	KarnanDevice(const KarnanDevice&) = delete;
	void operator=(const KarnanDevice&) = delete;
	KarnanDevice(KarnanDevice&&) = delete;
	KarnanDevice& operator=(KarnanDevice&&) = delete;

	VkCommandPool GetCommandPool() { return _commandPool; }
	VkDevice Device() { return _device; }
	VkSurfaceKHR Surface() { return _surface; }
	QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(_physicalDevice); }
	VkQueue GraphicsQueue() { return _graphicsQueue; }
	VkQueue PresentQueue() { return _presentQueue; }
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);


	SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(_physicalDevice); }

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,VkMemoryPropertyFlags properties,	VkBuffer& buffer,VkDeviceMemory& bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void CreateImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties,VkImage& image,VkDeviceMemory& imageMemory);

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

	void CreateCommandPool();


};
#pragma once
#include "KarnanWindow.h"

#include "vulkan/vulkan.h"

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

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
	uint32_t _graphicsQueueFamily;
	VkQueue _graphicsQueue;
	uint32_t _presentQueueFamily;
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


	VkInstance& GetVkInstance() { return _instance; };
	VkPhysicalDevice& GetVkPhysicalInstance() { return _physicalDevice; };

	VkCommandPool GetCommandPool() { return _commandPool; }
	VkDevice Device() { return _device; }
	VkSurfaceKHR Surface() { return _surface; }
	QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(_physicalDevice); }
	VkQueue GraphicsQueue() { return _graphicsQueue; }
	uint32_t GraphicsQueueFamily() { return _graphicsQueueFamily; };
	VkQueue PresentQueue() { return _presentQueue; }
	uint32_t PresentQueueFamily() { return _presentQueueFamily; };
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